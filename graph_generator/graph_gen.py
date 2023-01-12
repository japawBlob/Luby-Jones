import copy
import random
import numpy as np
# max number of edges = N*N/2 - N


class Graph:
    def __init__(self):
        self.nodes = []
        self.number_of_nodes = 0
        self.number_of_edges = 0
        self.connection_probability = 0
        self.hubs = False
        self.number_of_hubs = 0
        self.hub_connection_probability = 0
        self.prob_or_static = "x"
        self.static_connections = 0

    def init(self):
        for n in range(self.number_of_nodes):
            self.nodes.append(Node())
        for i in range(self.number_of_hubs):
            self.nodes[i].hub = True

    def fill_graph(self):
        if prob_or_static == "s":
            self.static_fill_graph()
        if prob_or_static == "p":
            self.probability_fill_graph()

    def probability_fill_graph(self):
        for i, node in enumerate(self.nodes):
            for j in range(i):
                if node.hub or self.nodes[j].hub:
                    if random.random() < self.hub_connection_probability:
                        node.append(j)
                        self.number_of_edges += 1
                elif random.random() < self.connection_probability:
                    node.append(j)
                    self.number_of_edges += 1

    def static_fill_graph(self):
        # for i, node in enumerate(self.nodes):
        #     for j in range(i):
        #         if node.hub or self.nodes[j].hub:
        #             if random.random() < self.hub_connection_probability:
        #                 node.append(j)
        #                 self.number_of_edges += 1
        #         elif random.random() < self.connection_probability:
        #             node.append(j)
        #             self.number_of_edges += 1
        # connectible_nodes = copy.deepcopy(self.nodes)
        # for i, node in enumerate(connectible_nodes):
        #     neighbours = np.random.choice(self.number_of_nodes, size=self.static_connections-len(node.neighbours), replace=False)
        #     for neighbour in neighbours:
        #         if neighbour == i:
        #             continue
        #         if self.nodes[neighbour]:
        #         node.append(neighbour)
        pass

    def print_graphviz(self, file=True):
        if file is False:
            print("Graph G {")
            for i, node in enumerate(self.nodes):
                print("\t" + str(i) + " -- {", end="")
                for neighbour in node.neighbours:
                    print(str(neighbour) + " ", end="")
                print("}")
            print("}")
        else:
            with open("../graphviz/" + self.create_filename(".dot"), "w+") as f:
                f.write("Graph G {\n")
                for i, node in enumerate(self.nodes):
                    f.write("\t" + str(i) + " -- {")
                    for neighbour in node.neighbours:
                        f.write(str(neighbour) + " ")
                    f.write("}\n")
                f.write("}\n")

    def print(self, file=True):
        if file is False:
            print(self.number_of_nodes, self.number_of_edges)
            for i, node in enumerate(self.nodes):
                for neighbour in node.neighbours:
                    print(str(i), str(neighbour))
        else:
            with open("../data_in/" + self.create_filename(".in"), "w+") as f:
                f.write(str(self.number_of_nodes) + " " + str(self.number_of_edges) + "\n")
                for i, node in enumerate(self.nodes):
                    for neighbour in node.neighbours:
                        f.write(str(i) + " " + str(neighbour) + "\n")

    def create_filename(self, file_type):
        ret = ""
        if self.prob_or_static == "p":
            ret = str(self.number_of_nodes) + "n-" + str(self.connection_probability) + "p"
            if self.hubs:
                ret += "-" + str(self.number_of_hubs) + "h-" + str(self.hub_connection_probability) + "hp"
            ret += file_type
        if self.prob_or_static == "s":
            ret = str(self.number_of_nodes) + "n-" + str(self.static_connections) + "s"
            if self.hubs:
                ret += "-" + str(self.number_of_hubs) + "h-" + str(self.hub_connection_probability) + "hp"
            ret += file_type
        return ret

class Node:
    def __init__(self):
        self.hub = False
        self.neighbours = []

    def __str__(self) -> str:
        return str(self.neighbours)

    def append(self, i):
        self.neighbours.append(i)


if __name__ == '__main__':
    g = Graph()

    print("This is basic program aimed to generate random graph")

    g.number_of_nodes = int(input("Enter number of nodes: "))
    prob_or_static = (input("Do you want to generate graph based on probability or static connections? \n"
                            "Enter \"p\" for probability and \"s\" for static"))
    if g.prob_or_static == "p" or g.prob_or_static == "P":
        g.connection_probability = float(input("please enter probability of connection between two nodes. "
                                       "Input value between 0 and 1.\n 1 for fully connected and 0 for null graph.: "))
        g.hubs = input("Do you want to create hubs? [Y/n]: ")
        # print(g.hubs)
        if g.hubs == "Y" or g.hubs == "y":
            g.hubs = True
            g.number_of_hubs = int(input("enter number of hubs: "))
            g.hub_connection_probability = float(input("please enter probability of hub connection: "))
        else:
            g.hubs = False
    else:
        g.static_connections = float(input("please enter unsigned integer, that represents number of neighbours does each node have"))



    print("initializing graph")
    g.init()
    print("Filling graph")
    g.fill_graph()
    print("Printing graphviz")
    g.print_graphviz()
    print("Printing")
    g.print()
