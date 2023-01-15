# Graph generator

This is really basic python script, that is used for generating graph inputs for main program.
This utility is really basic - for example there are no input checks during user input phase, 
so please keep that in mind when receiving different results, than you expect. 
This python script is slow, please keep that in mind, when generating your own files. 

In order to build a graph, this utility needs number of nodes and number between 1 and 0 (inclusive)
that represents probability, that between every possible pair of nodes will be edge. If you'd enter 
probability of 0.5 - when new node will be created, it will generate number for each currently existing
node, and if that number is greater than 0.5 - connection between new node and existing node will be created.

This utility allows you to generate two different types on nodes, each with different probability of connection.
I call these nodes hubs. Their functionality is same as basic node, only difference is, they have different
chance of connection, than basic nodes.

Created file name represents what input was used for generating that file. The basic formula is as follows:
```
$(NODES)n-$(PROBABILITY)p-$(HUBS)h-$(HUB_PROBABILITY)hp.in
``` 
where NODES is number of nodes, PROBABILITY is probability of connection for node HUBS is number of hub nodes and
HUB_PROBABILITY is connection creation probability of hubs.

For example STAR topology could be represented as follows
```
5n-0.0p-1h-1.0hp.in
```

This utility generates two files. Input files for main program - those files are put into ../data_in/ folder.
And .dot files, that cen be used for visualisation using graphviz - graphviz files are put into ../graphviz/ folder.