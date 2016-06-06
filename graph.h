#ifndef SHORTESTPATH_GRAPH_H
#define SHORTESTPATH_GRAPH_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <string>

/*
 * Exception for trying to find
 * a path between two nodes if
 * at least one of the nodes
 * doesn't exist.
 */
class NonExistentNodeException : public std::exception
{
public:
	virtual const char* what() const throw() {
		return "At least one of those nodes doesn't exist!";
	}
};

/*
 * Exception for trying to find
 * a path between two nodes when
 * no path exists.
 */
class NoPathException : public std::exception
{
public:
	virtual const char* what() const throw() {
		return "No path exists between those two nodes!";
	}
};

/*
 * Node
 * -----
 * Represents a node in a graph. T is
 * the type of value held in the node.
 */
template <typename T>
class Node
{
public:
	Node(const T& value) : value(value) {}

	/*
	 * Why is an unordered_set better than
	 * a vector for the list of adjacent
	 * nodes?
	 */
	std::unordered_set<Node<T>*> adjacents;
	T value;
	bool marked;
};

/*
 * Graph
 * -----
 * Represents a bi-directional (undirected)
 * graph. Nodes can have values of any type,
 * so long as that type:
 *   - can be used as a key in a map, and;
 *   - can be inserted into cout.
 *
 * The graph does not have to be connected.
 * Values must be unique.
 */
template <typename T>
class Graph
{
public:
	Graph() {}

	/*
	 * Since we dynamically allocate each node,
	 * we need the big 3!
	 *
	 * - destructor
	 * - copy constructor
	 * - assignment operator
	 */
	~Graph() {
		clear();
	}

	Graph(const Graph<T>& other) {
		copyOther(other);
	}

	Graph<T>& operator=(const Graph<T>& other) {
		if (this != &other) {
			clear();
			copyOther(other);
		}
		return *this;
	}

	/*
	 * Common graph operations.
	 */
	Node<T>* insert(const T& value) {
		if (nodes.find(value) != nodes.end()) {
			return NULL;
		}

		Node<T>* newNode = new Node<T>(value);
		nodes[value] = newNode;
		return newNode;
	}

	// Two versions of connect! One that takes
	// node pointers, another that takes node
	// values.
	void connect(Node<T>* first, Node<T>* second) {
		first->adjacents.insert(second);
		second->adjacents.insert(first);
	}

	void connect(const T& first, const T& second) {
		if (nodes.find(first) == nodes.end() ||
		    nodes.find(second) == nodes.end()) {
			throw NonExistentNodeException();
		}

		connect(nodes[first], nodes[second]);
	}

	// Unmarks all nodes. Important for the shortest
	// path algorithm!
	void unmarkAll() {
		for (auto iter = nodes.begin();
		     iter != nodes.end();
				 iter++) {
			iter->second->marked = false;
		}
	}

	// Prints a list of nodes in the graph, and
	// prints a list of adjacent nodes next to each
	// node.
	void print() {
		for (auto iter = nodes.begin();
		     iter != nodes.end();
				 iter++) {
			std::cout << iter->first << ": ";

			for (auto neighborsIter = iter->second->adjacents.begin();
			          neighborsIter != iter->second->adjacents.end();
								neighborsIter++) {

				std::cout << (*neighborsIter)->value << " ";
			}
			std::cout << std::endl;
		}
	}

	std::vector<Node<T>*> shortestPath(const T& start, const T& end) {

		// Make sure both nodes exist! If one doesn't, throw
		// the appropriate exception.
		// Ok, both nodes exist. Get the node pointers from
		// your unordered_map!
		// You will need some way to store partial paths.
		// I highly recommend representing a partial
		// path as a vector of node pointers, since this
		// is what you will ultimately be returning.
		//
		// I would ALSO recommend using a queue to store
		// all your partial paths.
		//
		// If you choose to use a queue (which you should),
		// you'll have a queue of vectors, where each vector
		// in turn contains node pointers. Kind of
		// complicated! But effective!
		// Don't forget to unmark all your nodes at the
		// beginning!

		if ( nodes.find(start) == nodes.end () || nodes.find(end) == nodes.end()){
			throw NonExistentNodeException();
		}

		std::queue< std::vector <Node<T>*>> path;
		std::vector<Node<T>*> first, second;
		first.push_back(new Node<T>(nodes.at(start)-> value));
		path.push(first);
		unmarkAll();

		// Now, repeatedly do the following:
		//    1. Get the topmost partial path.
		//    2. Look at every node you can get to from
		//       the end of that partial path (ignoring
		//       nodes you've already seen).
		//    3. Create a partial path that has the same
		//       nodes as the current partial path, PLUS
		//       one more node (the node that you're
		//       looking at).
		//    4. Store those partial paths at the end.
		//

		// You stop when:
		//    - You see the destination node at the end of
		//      the partial path you just removed from
		//      the queue. You can just return this path.
		//    - You run out of partial paths to look at.
		//      If this ever happens, that means there is
		//      no path to the end node. Throw the
		//      appropriate exception.

		while(true){
			if(!path.empty()) {
				first = path.front();
				path.pop();
			} else {
				throw NoPathException();
			}
			Node<T> *g = nodes.at(first.back() ->value);
			auto adj2 = g -> adjacents;
			for(auto iter = adj2.begin(); iter != adj2.end(); iter ++){
				if((*iter) -> marked == false){
					(*iter) -> marked = true;
					second = first;
					auto t = (*iter) -> value;
					second.push_back(new Node<T> (t));
					path.push(second);
					if(t == end){
						return second;
					}

				}
			}
		}



	}

private:
	std::unordered_map<T, Node<T>*> nodes;  // for uniqueness and lookup

	void copyOther(const Graph<T>& other) {

		// First, insert each node as an island
		// (no edges yet).
		for (auto iter = other.nodes.begin();
		     iter != other.nodes.end();
				 iter++) {
			insert(iter->first);
		}

		// Then, connect each pair of nodes that
		// has an edge in the other graph.
		for (auto iter = other.nodes.begin();
		     iter != other.nodes.end();
				 iter++) {
			auto adjacents = iter->second->adjacents;
			for (auto neighborsIter = adjacents.begin();
			     neighborsIter != adjacents.end();
					 ++neighborsIter) {
				// Need to connect the values here!
				// Connecting iter->second and *neighborsIter
				// would re-connect the nodes in the old graph
				// (so this would do nothing)
				connect(iter->first, (*neighborsIter)->value);
			}
		}
	}

	void clear() {
		for (auto iter = nodes.begin();
		     iter != nodes.end();
				 iter++) {
			delete iter->second;
		}

		nodes.clear();
	}
};

#endif

