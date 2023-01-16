#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>

#ifndef INPUT_FILE
    #define INPUT_FILE "../data_in/10n-0.2p-2h-0.5hp.in"
#endif
#if  defined(OPENMP) || defined(OPENMPI) || defined(THREAD)
    #include <atomic>
#endif

#ifdef THREAD
    #define THREAD_COUNT std::thread::hardware_concurrency()
    #include <thread>
    #include <mutex>
#endif
#if  defined(OPENMP) || defined(OPENMPI)
    #include <omp.h>
#endif
#ifdef OPENMPI
    #include "mpi.h"
#endif

#include "colours.h"

static std::string mode () {
#ifdef THREAD
    return "std::thread";
#elif defined(OPENMP)
    return "OpenMP";
#elif defined(OPENMPI)
    return "OpenMPI";
#else
    return "Serial";
#endif
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
    unsigned number_of_nodes, number_of_connections, number_of_colours, chunk_size, myid, numprocs;
    std::vector<node> nodes;

    graph() : number_of_colours(0) {
        load_nodes(std::cin);
#ifdef THREAD
        this->to_be_colored_lock = new std::mutex();
        this->chunk_size = (this->number_of_nodes+THREAD_COUNT-1) / THREAD_COUNT;
#endif
    }
    graph(const std::string& filename) : number_of_colours(0){
        std::fstream in (filename);
        if( in.fail() ){
            std::cerr << "File not found\n";
        }
        load_nodes(in);
#ifdef THREAD
        this->to_be_colored_lock = new std::mutex();
        this->chunk_size = (this->number_of_nodes+THREAD_COUNT-1) / THREAD_COUNT;
#endif
    }

#if defined (OPENMPI)
    graph(unsigned myid, unsigned numprocs) : number_of_colours(0), myid(myid), numprocs(numprocs) {
        load_nodes(std::cin);
        this->chunk_size = (number_of_nodes+numprocs-1)/numprocs;
    }
    graph(const std::string& filename, unsigned myid, unsigned numprocs) : number_of_colours(0), myid(myid), numprocs(numprocs){
        std::fstream in (filename);
        if( in.fail() ){
            std::cerr << "File not found\n";
        }
        load_nodes(in);
        this->chunk_size = (number_of_nodes+numprocs-1)/numprocs;
    }
#endif
#ifdef THREAD
    ~graph(){
        delete this->to_be_colored_lock;
    }
#endif
    void print_graphviz() const {
        std::ofstream out("../graphviz/" + std::to_string(this->number_of_nodes) + "n.dot");
        this->print_graphviz(out);
    }
    void print_graphviz(std::ostream& out) const {
        out << "graph {\n";
        for (size_t i = 0; i < nodes.size(); i++){
            out << nodes[i].to_string(i);
        }
        out << "}";
    }
    void compute(unsigned number_of_iterations){
        if (this->number_of_nodes == 0){
            return ;
        }
        unsigned total_time = 0;
        for (size_t i = 0; i< number_of_iterations; i++){
            this->clear_graph();
            auto start = std::chrono::high_resolution_clock::now();
#ifdef THREAD
            this->colour_graph_parallel();
#elif defined(OPENMP)
            this->colour_graph_parallel_openMP();
#elif defined(OPENMPI)
            this->colour_graph_parallel_mpi();
#else
            this->colour_graph();
#endif
            auto end = std::chrono::high_resolution_clock::now();
            total_time += to_ms(end - start).count();
#ifdef OPENMPI
            if( this->myid == 0)
#endif
            std::cout << "iteration number: " << i << "    Needed " << to_ms(end - start).count() << " ms to finish " << mode() << ".\n";
        }
#ifdef OPENMPI
        if( this->myid == 0)
#endif
        std::cout << "Average time needed for "<< mode() <<" compute: " << total_time/number_of_iterations <<"ms. Number of colours: " << this->number_of_colours << "\n\n";
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
#ifdef THREAD
    void colour_graph_parallel() {
        std::atomic<unsigned> number_of_coloured_nodes(0);
        while (number_of_coloured_nodes < this->number_of_nodes) {
            std::vector<unsigned> to_be_colored;

            std::vector<std::thread> threads;
            for (unsigned i = 0; i < THREAD_COUNT; i++) {
                threads.emplace_back(&graph::colour_graph_compute_parallel, this, std::ref(number_of_coloured_nodes),
                                     std::ref(to_be_colored), i);
            }
            for (auto &thread: threads) {
                thread.join();
            }
            for (const auto n: to_be_colored) {
                this->nodes[n].colour = this->number_of_colours;
            }
            this->number_of_colours++;
        }
    }
#endif
#ifdef OPENMP
    void colour_graph_parallel_openMP(){
        std::atomic<unsigned> number_of_coloured_nodes(0);
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored;
#pragma omp parallel for
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
#pragma omp critical
                    {
                        to_be_colored.push_back(n.name);
                    }
                    number_of_coloured_nodes++;
                }
            }
            for (const auto n : to_be_colored) {
                this->nodes[n].colour = this->number_of_colours;
            }
            this->number_of_colours++;
        }
    }
#endif
#ifdef OPENMPI
    void colour_graph_parallel_mpi(){
        std::atomic<unsigned> number_of_coloured_nodes(0);
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored;
#pragma omp parallel for
            for (unsigned i = myid*(chunk_size); i < std::min(myid*chunk_size+chunk_size, number_of_nodes); i++) {
                const auto & n = this->nodes[i];
                if (n.colour != -1) continue;
                bool local_max = true;
                for (const auto& neighbour : n.neighbours){
                    if (this->nodes[neighbour].colour == -1 && n.random_id < this->nodes[neighbour].random_id){
                        local_max = false;
                        break;
                    }
                }
                if (local_max) {
#pragma omp critical
                    {
                        to_be_colored.push_back(n.name);
                    }
                }
            }
            unsigned temp = to_be_colored.size();
            to_be_colored.resize(chunk_size);
            unsigned * complete_data = (unsigned*)malloc(number_of_nodes*sizeof(unsigned)+chunk_size*sizeof(unsigned));
            unsigned * coloured_per_process = (unsigned*)malloc(numprocs*sizeof(unsigned)+sizeof(unsigned));
            MPI_Allgather(&temp, 1, MPI_UNSIGNED, coloured_per_process, 1, MPI_UNSIGNED, MPI_COMM_WORLD);
            MPI_Allgather(&to_be_colored[0], chunk_size, MPI_UNSIGNED, complete_data, chunk_size, MPI_UNSIGNED, MPI_COMM_WORLD);
            for (unsigned i = 0; i<numprocs; i+=1){
                for (unsigned k = 0; k < coloured_per_process[i]; k++){
                    this->nodes[complete_data[i*chunk_size+k]].colour = this->number_of_colours;
                }
            }
            for (unsigned i = 0; i < numprocs; i++){
                number_of_coloured_nodes += coloured_per_process[i];
            }
            this->number_of_colours++;
            free(complete_data);
            free(coloured_per_process);
        }
    }
#endif
private:
#ifdef THREAD
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
                to_be_colored.push_back(n.name);
                this->to_be_colored_lock->unlock();
                number_of_coloured_nodes++;
            }
        }
    }
#endif
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


int main(int argc, char *argv[]) {
    std::string input_file;
    if(argc > 1){
        input_file = argv[1];
    } else {
        input_file = INPUT_FILE;
    }
#ifdef OPENMPI
    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    graph g = graph(input_file, myid, numprocs);
    if (myid == 0) std::cout << "loading done" << std::endl;
#else
    graph g = graph(input_file);
    std::cout << "loading done" << std::endl;
#endif
    size_t number_of_iterations = 5;
    g.compute(number_of_iterations);

#ifdef OPENMPI
    MPI_Finalize();
#endif
    return 0;
}
