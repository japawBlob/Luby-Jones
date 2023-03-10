#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include "colours.h"
#include <thread>
#include <mutex>
#include <unistd.h>
#include <cstring>

#define THREAD_COUNT std::thread::hardware_concurrency()

enum class Mode {
    Serial, Parallel
};

static std::string print_mode (Mode m){
    switch(m){
        case (Mode::Serial):
        {
            return "Serial";
        }
        case (Mode::Parallel):
        {
            return "Parallel";
        }
    }
    return "";
}

template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

struct node {
    unsigned name;
    int colour;
    unsigned random_id;
    std::vector<unsigned> neighbours;

    node() = default;
    node(unsigned name, unsigned random_id) : name (name), colour(-1), random_id(random_id)
    {};
    void print_n() const {
        std::cout << "name: " << name << "\nrand: "<< random_id << "\ncolour: " << colour << "\n\n";
    }
    std::string to_string () const {
        std::string s = std::to_string(this->name);
        s += " [style=\"filled\", color=\"";
        s += colors::get_colour(this->colour) + "\"]\n";
        s += std::to_string(this->name) + " -- {";
        for (const auto& n : this->neighbours){
            s += std::to_string(n) + " ";
        }
        s += "}\n\n";

        return s;
    }
    std::string to_string (unsigned i) const {
        std::string s = std::to_string(this->name);
        s += " [style=\"filled\", color=\"";
        s += colors::get_colour(this->colour) + "\"]\n";
        s += std::to_string(this->name) + " -- {";
        for (const auto& n : this->neighbours){
            if (n > i){
                s += std::to_string(n) + " ";
            }
        }

        s += "}\n\n";

        return s;
    }
};


struct graph {
    unsigned number_of_nodes, number_of_connections, number_of_colours, chunk_size;
    std::vector<node> nodes;
    graph() : number_of_colours(0) {
        load_nodes(std::cin);
        this->to_be_colored_lock = new std::mutex();
        this->chunk_size = (this->number_of_nodes+THREAD_COUNT-1) / THREAD_COUNT;
    }
    graph(const std::string& filename) : number_of_colours(0){
        if (filename.empty()) {
            load_nodes(std::cin);
        } else {
            std::fstream in (filename);
            load_nodes(in);
        }
        std::cout << "loading done\n";
        this->to_be_colored_lock = new std::mutex();
        this->chunk_size = (this->number_of_nodes+THREAD_COUNT-1) / THREAD_COUNT;
    }
    ~graph(){
        delete this->to_be_colored_lock;
    }
    void print_graphviz() const {
        std::ofstream out("../graphviz/coloured-" + std::to_string(this->number_of_nodes) + "n.dot");
        this->print_graphviz(out);
    }
    void print_graphviz(std::ostream& out) const {
        out << "graph {\n";
        for (size_t i = 0; i < nodes.size(); i++){
            out << nodes[i].to_string(i);
        }
        out << "}";
    }
    void compute(unsigned number_of_iterations, Mode m){
        unsigned total_time = 0;
        for (size_t i = 0; i< number_of_iterations; i++){
            this->clear_graph();
            auto start = std::chrono::high_resolution_clock::now();
            switch (m) {
                case(Mode::Serial): {
                    this->colour_graph();
                    break;
                }
                case(Mode::Parallel): {
                    this->colour_graph_parallel();
                    break;
                }
                default:
                {
                    std::cerr << "Mode not supported\n";
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            total_time += to_ms(end - start).count();
            std::cout << "iteration number: " << i << "    Needed " << to_ms(end - start).count() << " ms to finish " << print_mode(m) << ".\n";
        }
        std::cout << "Average time needed for "<< print_mode(m) <<" compute: " << total_time/number_of_iterations <<"ms. Number of colours: " << this->number_of_colours << std::endl;
        std::cout << std::endl;
    }

    void clear_graph(){
        for (auto & blob : this->nodes){
            blob.colour = -1;
        }
        this->number_of_colours = 0;
    }
    void colour_graph(){
        unsigned number_of_coloured_nodes = 0;
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored;
            for (const auto& n : this->nodes) {
                if (n.colour != -1) continue;
                bool local_max = true;
                for (const auto& neighbour : n.neighbours){
                    if (this->nodes[neighbour].colour == -1 && n.random_id < this->nodes[neighbour].random_id){
                        local_max = false;
                        break;
                    }
                }
                if (local_max) {
                    to_be_colored.push_back(n.name);
                    number_of_coloured_nodes++;
                }
            }
            for (const auto n : to_be_colored) {
                this->nodes[n].colour = this->number_of_colours;
            }
            this->number_of_colours++;
        }
    }
    void colour_graph_parallel(){
        std::atomic<unsigned> number_of_coloured_nodes(0);
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored;

            std::vector<std::thread> threads;
            for (unsigned i = 0; i<THREAD_COUNT; i++){
                threads.emplace_back(&graph::colour_graph_compute_parallel, this, std::ref(number_of_coloured_nodes), std::ref(to_be_colored), i);
            }
            for (auto & thread : threads){
                thread.join();
            }
            for (const auto n : to_be_colored) {
                this->nodes[n].colour = this->number_of_colours;
            }
            this->number_of_colours++;
        }
    }
private:
    std::mutex * to_be_colored_lock;
    void colour_graph_compute_parallel(std::atomic<unsigned> & number_of_coloured_nodes, std::vector<unsigned> & to_be_colored, unsigned id){
        for (unsigned i = id*(this->chunk_size); i < std::min(id*this->chunk_size+this->chunk_size, this->number_of_nodes); i++) {
            node & n = this->nodes[i];
            if (n.colour != -1) continue;
            bool local_max = true;
            for (const auto& neighbour : n.neighbours){
                if (this->nodes[neighbour].colour == -1 && n.random_id < this->nodes[neighbour].random_id){
                    local_max = false;
                    break;
                }
            }
            if (local_max) {
                this->to_be_colored_lock->lock();
                to_be_colored.push_back(i);
                this->to_be_colored_lock->unlock();
                number_of_coloured_nodes++;
            }
        }
    }
    void load_nodes(std::istream& in){
        in >> this->number_of_nodes >> this->number_of_connections;
        for (unsigned i = 0; i < this->number_of_nodes; i++){
            this->nodes.emplace_back(i, rand());
        }
        for (unsigned i = 0; i < this->number_of_connections; i++){
            unsigned from, to;
            in >> from >> to;
            this->nodes[from].neighbours.push_back(to);
            this->nodes[to].neighbours.push_back(from);
        }
    }
};

struct arguments{
    std::string file_name;
    Mode mode;
    unsigned number_of_iterations;

    arguments() : mode(Mode::Serial), number_of_iterations(5)
    {};
};

arguments handle_arguments(int argc, char *argv[]){
    if (argc < 2){
        return {};
    }
    if ( strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 ){
        std::cout << "This is basic program which implements Luby_Jones algorithm. For more advanced approach look into openMPI folder"
                     "\nProgram supports following flags\n"
                     "\t-f [file_name] to specify path to input file. Default is accepting input from std::in.\n"
                     "\t-p to enable parallel computation. Default is computation in serial\n"
                     "\t-i [number_of_iterations] to specify how many times the computation should be executed. Default number is 5\n";
        exit(0);
    }
    arguments a;
    int opt;
    while((opt = getopt(argc, argv, ":f:pi:")) != -1)
    {
        switch (opt) {
            case('f'):
            {
                a.file_name = optarg;
                break;
            }
            case('p'):
            {
                a.mode = Mode::Parallel;
                break;
            }
            case('i'):
            {
                a.number_of_iterations = atoi(optarg);
                break;
            }
            default:
            {
                std::cerr << "flag not supported\n";
                exit(-1);
            }
        }
    }
    return a;
}

int main(int argc, char *argv[]) {
    arguments args = handle_arguments(argc, argv);
    graph g = graph(args.file_name);

    g.compute(args.number_of_iterations, args.mode);

    g.print_graphviz();
    return 0;
}
