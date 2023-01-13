#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <atomic>
#include <omp.h>
#include <chrono>
#include "colours.h"
#include "mpi.h"

struct node {
    unsigned name;
    unsigned colour;
    unsigned random_id;
    std::vector<unsigned> neighbours;

    node() = default;
    node(unsigned name, unsigned random_id) : name (name), colour(0), random_id(random_id)
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
        for ( auto n = this->neighbours.begin(); n <= this->neighbours.begin() + i; n++ ){
            s += std::to_string(*n) + " ";
        }

        s += "}\n\n";

        return s;
    }
};

struct graph {
    unsigned number_of_nodes, number_of_connections, number_of_colours, chunk_size, myid, numprocs;
    std::vector<node> nodes;

    graph(unsigned myid, unsigned numprocs) : number_of_colours(0), myid(myid), numprocs(numprocs) {
        load_nodes(std::cin);
        this->chunk_size = (number_of_nodes+numprocs-1)/numprocs;
    }
    graph(const std::string& filename, unsigned myid, unsigned numprocs) : number_of_colours(0), myid(myid), numprocs(numprocs){
        std::fstream in (filename);
        load_nodes(in);
        this->chunk_size = (number_of_nodes+numprocs-1)/numprocs;
    }
    void print_graphviz(std::ostream& out) const {
        for (const auto& n : nodes) {
            out << n.to_string();
        }
    }
    void clear_graph(){
        for (auto & blob : this->nodes){
            blob.colour = 0;
        }
        this->number_of_colours = 0;
    }
    void colour_graph(){
        unsigned number_of_coloured_nodes = 0;
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored;
            for (const auto& n : this->nodes) {
                if (n.colour != 0) continue;
                bool local_max = true;
                for (const auto& neighbour : n.neighbours){
                    if (n.colour == 0 && this->nodes[neighbour].colour == 0 &&
                        n.random_id < this->nodes[neighbour].random_id){ // lze optimalizovat
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
#pragma omp parallel for
            for (const auto& n : this->nodes) {
                if (n.colour != 0) continue;
                bool local_max = true;
                for (const auto& neighbour : n.neighbours){
                    if (this->nodes[neighbour].colour == 0 &&
                        n.random_id < this->nodes[neighbour].random_id){ // lze optimalizovat
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
    void colour_graph_parallel_mpi(){
        std::atomic<unsigned> number_of_coloured_nodes(0);
        while (number_of_coloured_nodes < this->number_of_nodes){
            std::vector<unsigned> to_be_colored(chunk_size);
#pragma omp parallel for
            for (unsigned i = myid*(chunk_size); i < std::min(myid*chunk_size+chunk_size, number_of_nodes); i++) {
                const auto & n = this->nodes[i];
                if (n.colour != 0) continue;
                bool local_max = true;
                for (const auto& neighbour : n.neighbours){
                    if (this->nodes[neighbour].colour == 0 &&
                        n.random_id < this->nodes[neighbour].random_id){ // lze optimalizovat
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
            unsigned temp = number_of_coloured_nodes;
            unsigned * complete_data = (unsigned*)malloc(number_of_nodes*sizeof(unsigned)+chunk_size*sizeof(unsigned));
            unsigned * counts_per_process = (unsigned*)malloc(numprocs*sizeof(unsigned)+sizeof(unsigned));
            MPI_Allgather(&temp, 1, MPI_UNSIGNED, counts_per_process, 1, MPI_UNSIGNED, MPI_COMM_WORLD);
            MPI_Allgather(&to_be_colored[0], chunk_size, MPI_UNSIGNED, complete_data, chunk_size, MPI_UNSIGNED, MPI_COMM_WORLD);
            for (int i = 0; i<numprocs; i+=1){
                for (int k = 0; k < counts_per_process[i]; k++){
                    this->nodes[complete_data[i*chunk_size+k]].colour = this->number_of_colours;
                }
            }
            /// TODO SUM ALL NUMBERS OF COLOURED NODES

            // for (const auto n : to_be_colored) {
            //     this->nodes[n].colour = this->number_of_colours;
            // }
            this->number_of_colours++;
        }
    }    
private:
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


template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

int main(int argc, char *argv[]) {
    int myid, numprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // obtain number
                                            // of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);     // obtain process number

    graph g = graph("10n-0.5p.in", myid, numprocs);

    std::cout << "loading done" << std::endl;

    size_t number_of_iterations = 1;
    for (size_t i = 0; i< number_of_iterations; i++){
        g.clear_graph();
        auto parallel_start = std::chrono::high_resolution_clock::now();
        g.colour_graph_parallel(); // 500ms
        auto parallel_end = std::chrono::high_resolution_clock::now();
        std::cout << "iteration number: " << i << "    Needed " << to_ms(parallel_end - parallel_start).count() << " ms to finish parallel.\n";
    }
    std::cout << "number of colours: " << g.number_of_colours << std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i< number_of_iterations; i++){
        g.clear_graph();
        auto serial_start = std::chrono::high_resolution_clock::now();
        g.colour_graph(); // 1721 ms
        auto serial_end = std::chrono::high_resolution_clock::now();
        std::cout << "iteration number: " << i << "    Needed " << to_ms(serial_end - serial_start).count() << " ms to finish serial.\n";
    }
    std::cout << "number of colours: " << g.number_of_colours << std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i< number_of_iterations; i++){
        g.clear_graph();
        auto serial_start = std::chrono::high_resolution_clock::now();
        g.colour_graph_parallel_mpi(); // 1721 ms
        auto serial_end = std::chrono::high_resolution_clock::now();
        std::cout << "iteration number: " << i << "    Needed " << to_ms(serial_end - serial_start).count() << " ms to finish mpi.\n";
    }
    std::cout << "number of colours: " << g.number_of_colours << std::endl;


    // std::ofstream out("out.dot");
    // out.open("out.dot", "w+");
    // g.print_graphviz(out);
    MPI_Finalize();
    return 0;
}
