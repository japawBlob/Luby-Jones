#include <iostream>
#include <vector>
#include <cstdlib>
#include <atomic>

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
};

int main() {
    unsigned number_of_nodes, number_of_connections, i;
    std::cin >> number_of_nodes >> number_of_connections >> i >> i;

    std::cout << number_of_nodes << "\n";

    std::vector<node> nodes;
    for (unsigned i = 0; i < number_of_nodes; i++){
        nodes.emplace_back(i, rand());
    }
    for (unsigned i = 0; i < number_of_connections; i++){
        unsigned from, to;
        std::cin >> from >> to;
        nodes[from].neighbours.push_back(to);
        nodes[to].neighbours.push_back(from);
    }
    std::atomic<unsigned> number_of_coloured_nodes(0);
    unsigned colour = 1;
    while (number_of_coloured_nodes < number_of_nodes){
        std::vector<unsigned> to_be_colored;
#pragma omp parallel for
        for (auto& n : nodes) {
            if (n.colour != 0) continue;
            bool local_max = true;
            for (const auto& neighbour : n.neighbours){
                if (n.colour == 0 && nodes[neighbour].colour == 0 && n.random_id < nodes[neighbour].random_id){ // lze optimalizovat
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
        for (auto n : to_be_colored) {
            nodes[n].colour = colour;
        }
        colour++;
    }
    for (const auto& n : nodes) {
        n.print_n();
    }
    return 0;
}
