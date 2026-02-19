The task is to develop a generic class CNet that computes the fee to complete a request in a network of corrupted bureaucrats.

Assume a network of corrupted clerks. The clerks are organized in a tree-like structure. Some clerks are willing to cooperate and exchange documents for a fee. There is always exactly one connection between any pair of clerks in the bureaucratic network. The connection is either direct (the clerks cooperate), or there are some intermediate clerks in between. If we need our request to pass from clerk A to clerk B, we have to pay the fee accumulated along the way.

Class CNet is given a list of pairs, each pair consists of two clerks that cooperate. Moreover, the fee is given for that cooperation. The cooperation is always bi-directional, the fee is the same for both directions.

To preserve the privacy of the clerks, the participating clerks are not identified by their real names. Instead, they may be represented by general strings, numbers, or other objects. Thus, the implemented class CNet must be a generic class, the generic parameter is the type that represents the clerks.

### **Class CNet**

- default constructor

    initializes an empty instance.

- add ( y, y, cost )

    method adds a pair of cooperating clerks x and y, the cost to pay for the cooperation is cost. Return value is a reference to the CNet instance (fluent interface).

- optimize ()

    the method is called after the last invocation of add but before the first invocation of totalCost. Use the method to pre-calculate some internal structures that speed up the further calls to totalCost. Return value is a reference to the CNet instance (fluent interface).

- totalCost(x,y)

    the method computes the total cost needed to pay such that our request is passed from clerk x to clerk y. The computed cost is returned as an integer. If the connection does not exist (an unknown clerk), the return value is -1.

- T_

    a generic parameter of the class, the clerks are represented by this type. The type may provide only a very basic interface: copying, moving, destruction, and equality/inequality comparison (==, !=). Other operations may exist, however, they are not guaranteed. An example of such minimalist type is class CDumbString in the attached archive.
