#ifndef GRAPH_H_
#define GRAPH_H_

#include "MutablePriorityQueue.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

template <class T> class Edge;
template <class T> class Graph;
template <class T> class Vertex;

#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

template <class T> struct eqhashEdge {
  bool operator()(const Edge<T> &e1, const Edge<T> &e2) const {
    // cout<<e1.orig<<"\t"<<e2.orig<<"\t"<<e1.dest<<"\t"<<e2.dest<<endl;
    return e1.orig == e2.orig && e1.dest == e2.dest;
  }
};

template <class T> struct hhashEdge {
  int operator()(const Edge<T> &e1) const {
    hash<T> hash_T;
    return hash_T(e1.orig->getInfo() + e1.dest->getInfo());
  }
};

template <class T> class Vertex {

  T info; // contents

  // vector<Edge<T> > adj;  // outgoing edges
  std::unordered_set<Edge<T>, hhashEdge<T>, eqhashEdge<T>> edgeHashTable;

  bool visited; // auxiliary field
  double dist = 0;
  Vertex<T> *path = nullptr;
  int queueIndex = 0; // required by MutablePriorityQueue

  void addProcessedEdge(Vertex<T> *dest, vector<Vertex<T> *> path);
  void addEdge(Vertex<T> *dest, double w);
  void addEdge(Vertex<T> *dest);
  bool removeEdge(const T &dest);

  bool position = false;
  double x = 0, y = 0;
  // std::string name;

public:
  Vertex(T in);
  Vertex(T in, int x, int y);
  bool operator<(Vertex<T> &vertex) const; // // required by MutablePriorityQueue
  T getInfo() const;
  double getDist() const;
  Vertex *getPath() const;
  vector<Edge<T>> getEdges() const;
  vector<Vertex<T> *> getProcessedEdge(Vertex<T> *dest);

  double getX() const;
  double getY() const;
  bool hasPosition() const;

  double getEdgeWeight(Vertex<T> *dest);

  friend class Graph<T>;
  friend class MutablePriorityQueue<Vertex<T>>;
};

template <class T> Vertex<T>::Vertex(T in) : info(in), visited(false) {}

template <class T> Vertex<T>::Vertex(T in, int x, int y) : info(in) {
  this->x = x;
  this->y = y;
  this->position = true;
  this->visited = false;
}

template <class T> double Vertex<T>::getX() const { return x; }
template <class T> double Vertex<T>::getY() const { return y; }
template <class T> bool Vertex<T>::hasPosition() const { return position; }
template <class T>

double Vertex<T>::getEdgeWeight(Vertex<T> *dest) {
  if (this == dest)
    return 0;

  Edge<T> e(this, dest);
  typename unordered_set<Edge<T>, hhashEdge<T>, eqhashEdge<T>>::const_iterator
      it = edgeHashTable.find(e);
  if (it == edgeHashTable.end()) {
    // cout<<"a"<<endl;
    return INF;
  }
  return it->getWeight();
}

/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge weight (w).
 */
template <class T>
void Vertex<T>::addProcessedEdge(Vertex<T> *d, vector<Vertex<T> *> p) {
  edgeHashTable.insert(Edge<T>(this, d, p));
}

template <class T> void Vertex<T>::addEdge(Vertex<T> *d, double w) {
  // adj.push_back(Edge<T>(this, d, w));
  edgeHashTable.insert(Edge<T>(this, d, w));
}

template <class T> void Vertex<T>::addEdge(Vertex<T> *d) {
  // adj.push_back(Edge<T>(this, d));
  edgeHashTable.insert(Edge<T>(this, d));
}

template <class T> bool Vertex<T>::removeEdge(const T &dest) {
	Edge<T> e(this, dest);

	if(edgeHashTable.erase(edgeHashTable.find(e))==0)
		return false;
	return true;
}

template <class T> bool Vertex<T>::operator<(Vertex<T> &vertex) const {
  return this->dist < vertex.dist;
}

template <class T> vector<Edge<T>> Vertex<T>::getEdges() const {
  vector<Edge<T>> v;
  for (auto it = edgeHashTable.begin(); it != edgeHashTable.end(); ++it)
    v.push_back(*it);
  return v;
}

template <class T>
vector<Vertex<T> *> Vertex<T>::getProcessedEdge(Vertex<T> *dest) {
  Edge<T> e(this, dest);
  typename unordered_set<Edge<T>, hhashEdge<T>, eqhashEdge<T>>::iterator it =
      edgeHashTable.find(e);
  if (it == edgeHashTable.end()) {
    return vector<Vertex<T> *>();
  }
  Edge<T> edge = *it;
  return edge.processedEdge;
}

template <class T> T Vertex<T>::getInfo() const { return this->info; }

template <class T> double Vertex<T>::getDist() const { return this->dist; }

template <class T> Vertex<T> *Vertex<T>::getPath() const { return this->path; }

/********************** Edge  ****************************/

template <class T> class Edge {

  static unsigned int currentEdge;

  double weight = 0; // edge weight

  // bool selected; // Fp07

  unsigned int edgeId;

public:
  Vertex<T> *orig = NULL; // Fp07
  Vertex<T> *dest = NULL; // destination vertex
  vector<Vertex<T> *> processedEdge;

  Edge(Vertex<T> *o, Vertex<T> *d, vector<Vertex<T> *> path);
  Edge(Vertex<T> *o, Vertex<T> *d, double w);
  Edge(Vertex<T> *o, Vertex<T> *d);
  friend class Graph<T>;
  friend class Vertex<T>;
  unsigned int getEdgeId() const;

  // Fp07
  double getWeight() const;
};

template <class T> unsigned int Edge<T>::currentEdge = 0;

template <class T>
Edge<T>::Edge(Vertex<T> *o, Vertex<T> *d, vector<Vertex<T> *> path)
    : edgeId(currentEdge++), orig(o), dest(d), processedEdge(path) {
  for (size_t k = 1; k < path.size(); k++) {
    weight += path.at(k - 1)->getEdgeWeight(path.at(k));
  }
}

template <class T>
Edge<T>::Edge(Vertex<T> *o, Vertex<T> *d, double w)
    : weight(w), edgeId(currentEdge++), orig(o), dest(d) {}

template <class T>
Edge<T>::Edge(Vertex<T> *o, Vertex<T> *d)
    : edgeId(currentEdge++), orig(o), dest(d) {
  weight = sqrt(pow(o->getX() - d->getX(), 2) + pow(o->getY() - d->getY(), 2));
}

template <class T> double Edge<T>::getWeight() const { return weight; }
template <class T> unsigned int Edge<T>::getEdgeId() const { return edgeId; }

/*************************** Graph  **************************/

template <class T> struct eqhash {
  bool operator()(const Vertex<T> *s1, const Vertex<T> *s2) const {
    return s1->getInfo() == s2->getInfo();
  }
};

template <class T> struct hhash {
  int operator()(const Vertex<T> *s1) const {
    hash<T> hash_T;
    return hash_T(s1->getInfo());
  }
};

template <class T> class Graph {
  // vector<Vertex<T> *> vertexSet;    // vertex set
  std::unordered_set<Vertex<T> *, hhash<T>, eqhash<T>> vertexHashTable;
  // std::unordered_map< T, Vertex<T> *, hhash<T>, eqhash<T> > vertexMap;

  // Fp05
  Vertex<T> *initSingleSource(const T &orig);
  bool relax(Vertex<T> *v, Vertex<T> *w, double weight);
  double **W = nullptr; // dist
  int **P = nullptr;    // path
  int findVertexIdx(const T &in) const;

public:
  Vertex<T> *findVertex(const T &in) const;
  bool addVertex(const T &in);
  bool addVertex(const T &in, double x, double y);
  bool removeVertex(Vertex<T> &v);
  bool removeVertex(const T &in);
  bool addProcessedEdge(const T &sourc, const T &dest, 
  							vector<Vertex<T> *> path);
  bool addEdge(const T &sourc, const T &dest, double w);
  bool addEdge(const T &sourc, const T &dest);
  bool removeEdge(const T &sourc, const T &dest);
  bool removeEdge(Edge<T> &e);
  double getWeight(T orig, T dest);
  int getNumVertex() const;
  vector<Vertex<T> *> getVertexSet() const;

  // Fp05 - single source
  void dijkstraShortestPath(const T &s);
  void unweightedShortestPath(const T &s);
  void bellmanFordShortestPath(const T &s);
  vector<T> getPath(const T &origin, const T &dest) const;
  vector<Vertex<T> *> getPathV(const T &origin, const T &dest) const;

  // Fp05 - all pairs
  void floydWarshallShortestPath();
  vector<T> getfloydWarshallPath(const T &origin, const T &dest) const;
  ~Graph();

  // Fp07 - minimum spanning tree
  vector<Vertex<T> *> calculatePrim();
  vector<Vertex<T> *> calculateKruskal();
};

template <class T> int Graph<T>::getNumVertex() const {
  return vertexHashTable.size();
}

template <class T> vector<Vertex<T> *> Graph<T>::getVertexSet() const {
  vector<Vertex<T> *> v;
  for (auto it = vertexHashTable.begin(); it != vertexHashTable.end(); ++it)
    v.push_back(*it);
  return v;
}

template <class T> double Graph<T>::getWeight(T orig, T dest) {
  Vertex<T> *v = findVertex(orig);
  if (v == nullptr)
    return INF;
  Vertex<T> *d = findVertex(dest);
  if (d == nullptr)
    return INF;
  double dist = v->getEdgeWeight(d);
  // cout<<dist<<endl;
  return dist;
}

/*
 * Auxiliary function to find a vertex with a given content.
 */
template <class T> Vertex<T> *Graph<T>::findVertex(const T &in) const {
  Vertex<T> v(in);
  typename unordered_set<Vertex<T> *, hhash<T>, eqhash<T>>::const_iterator it =
      vertexHashTable.find(&v);
  if (it == vertexHashTable.end())
    return nullptr;
  return *it;
}

/*
 * Finds the index of the vertex with a given content.
 */
template <class T> int Graph<T>::findVertexIdx(const T &in) const {
  /*for (unsigned i = 0; i < vertexSet.size(); i++)
          if (vertexSet[i]->info == in)
                  return i;*/
  return -1;
}
/*
 *  Adds a vertex with a given content or info (in) to a graph (this).
 *  Returns true if successful, and false if a vertex with that content already
 * exists.
 */
template <class T> bool Graph<T>::addVertex(const T &in) {
  if (findVertex(in) != nullptr)
    return false;
  Vertex<T> *v = new Vertex<T>(in);
  // vertexSet.push_back(v);
  vertexHashTable.insert(v);
  return true;
}

template <class T> bool Graph<T>::addVertex(const T &in, double x, double y) {
  if (findVertex(in) != nullptr)
    return false;
  Vertex<T> *v = new Vertex<T>(in, x, y);
  // vertexSet.push_back(v);
  vertexHashTable.insert(v);
  return true;
}

template <class T> bool Graph<T>::removeVertex(Vertex<T> &v) {
	if (findVertex(v->info) == nullptr)
		return false;
	vertexHashTable.erase(vertexHashTable.find(v));
  for (auto it=vertexHashTable.begin(); it!=vertexHashTable.end(); ++it) {
    vector<Edge<T>> v = it->getEdges();
    for(int i=0; i<v.size(); i++) {
      if(v.at(i).dest==v) {
        it->removeEdge(v);
      }
    }
  }
  /* Alternativa que até me parece melhor. Basta apenas isto, não há comparações
  // não há nada. Caso exista elimina, caso contrario ignora-se
  for (auto it=vertexHashTable.begin(); it!=vertexHashTable.end(); ++it) {
    it->removeEdge(v);
  }
  */
	return true;
}

template <class T> bool Graph<T>::removeVertex(const T &content) {
	if (findVertex(content)==nullptr)
		return false;
	vertexHashTable.erase(vertexHashTable.find(findVertex(content)));
	return true;
}

/*
 * Adds an edge to a graph (this), given the contents of the source and
 * destination vertices and the edge weight (w).
 * Returns true if successful, and false if the source or destination vertex
 * does not exist.
 */
template <class T>
bool Graph<T>::addProcessedEdge(const T &sourc, const T &dest,
                                vector<Vertex<T> *> path) {
  auto v1 = findVertex(sourc);
  auto v2 = findVertex(dest);
  if (v1 == nullptr || v2 == nullptr)
    return false;
  v1->addProcessedEdge(v2, path);
  return true;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
  auto v1 = findVertex(sourc);
  auto v2 = findVertex(dest);
  if (v1 == nullptr || v2 == nullptr)
    return false;
  v1->addEdge(v2, w);
  return true;
}

template <class T> 
bool Graph<T>::addEdge(const T &sourc, const T &dest) {
  auto v1 = findVertex(sourc);
  auto v2 = findVertex(dest);
  if (v1 == nullptr || v2 == nullptr)
    return false;
  v1->addEdge(v2);
  return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
	auto v1 = findVertex(sourc);
	auto v2 = findVertex(dest);
	if(v1 == nullptr || v2 == nullptr)
		return false;
	if(v1->removeEdge(v2)==false)
		return false;
	return true;
}

template <class T>
bool Graph<T>::removeEdge(Edge<T> &e) {
	Vertex<T> orig = e.orig;
	Vertex<T> dest = e.dest;
	if(orig.removeedge(dest)==false)
		return false;
	return true;
}

/**************** Single Source Shortest Path algorithms ************/

/**
 * Initializes single source shortest path data (path, dist).
 * Receives the content of the source vertex and returns a pointer to the source
 * vertex. Used by all single-source shortest path algorithms.
 */
template <class T> Vertex<T> *Graph<T>::initSingleSource(const T &origin) {
  for (auto v : vertexHashTable) {
    v->dist = INF;
    v->path = nullptr;
  }
  auto s = findVertex(origin);
  s->dist = 0;
  return s;
}

/**
 * Analyzes an edge in single source shortest path algorithm.
 * Returns true if the target vertex was relaxed (dist, path).
 * Used by all single-source shortest path algorithms.
 */
template <class T>
inline bool Graph<T>::relax(Vertex<T> *v, Vertex<T> *w, double weight) {
  if (v->dist + weight < w->dist) {
    w->dist = v->dist + weight;
    w->path = v;
    return true;
  } else
    return false;
}

template <class T> void Graph<T>::dijkstraShortestPath(const T &origin) {
  auto s = initSingleSource(origin);
  MutablePriorityQueue<Vertex<T>> q;
  q.insert(s);
  while (!q.empty()) {
    auto v = q.extractMin();
    for (auto e : v->edgeHashTable) {
      auto oldDist = e.dest->dist;
      if (relax(v, e.dest, e.weight)) {
        if (oldDist == INF)
          q.insert(e.dest);
        else
          q.decreaseKey(e.dest);
      }
    }
  }
}

template <class T>
vector<T> Graph<T>::getPath(const T &origin, const T &dest) const {
  vector<T> res;
  auto v = findVertex(dest);
  if (v == nullptr || v->dist == INF) // missing or disconnected
    return res;
  for (; v != nullptr; v = v->path)
    res.push_back(v->info);
  reverse(res.begin(), res.end());
  return res;
}

template <class T>
vector<Vertex<T> *> Graph<T>::getPathV(const T &origin, const T &dest) const {
  vector<Vertex<T> *> res;
  auto v = findVertex(dest);
  if (v == nullptr || v->dist == INF) // missing or disconnected
    return res;
  for (; v != nullptr; v = v->path)
    res.push_back(v);
  reverse(res.begin(), res.end());
  return res;
}

template <class T> void Graph<T>::unweightedShortestPath(const T &orig) {
  auto s = initSingleSource(orig);
  queue<Vertex<T> *> q;
  q.push(s);
  while (!q.empty()) {
    auto v = q.front();
    q.pop();
    for (auto e : v->adj)
      if (relax(v, e.dest, 1))
        q.push(e.dest);
  }
}

template <class T> void Graph<T>::bellmanFordShortestPath(const T &orig) {
  initSingleSource(orig);
  vector<Vertex<T>> vertexSet = getVertexSet();
  for (unsigned i = 1; i < vertexSet.size(); i++)
    for (auto v : vertexSet)
      for (auto e : v->adj)
        relax(v, e.dest, e.weight);
  for (auto v : vertexSet)
    for (auto e : v->adj)
      if (relax(v, e.dest, e.weight))
        std::cout << "Negative cycle!" << std::endl;
}

/**************** All Pairs Shortest Path  ***************/

template <class T> void deleteMatrix(T **m, int n) {
  if (m != nullptr) {
    for (int i = 0; i < n; i++)
      if (m[i] != nullptr)
        delete[] m[i];
    delete[] m;
  }
}

template <class T> Graph<T>::~Graph() {
  deleteMatrix(W, getVertexSet().size());
  deleteMatrix(P, getVertexSet().size());
}

template <class T> void Graph<T>::floydWarshallShortestPath() {
  unsigned n = getNumVertex();
  vector<Vertex<T>> vertexSet = getVertexSet();
  deleteMatrix(W, n);
  deleteMatrix(P, n);
  W = new double *[n];
  P = new int *[n];
  for (unsigned i = 0; i < n; i++) {
    W[i] = new double[n];
    P[i] = new int[n];
    for (unsigned j = 0; j < n; j++) {
      W[i][j] = i == j ? 0 : INF;
      P[i][j] = -1;
    }
    for (auto e : vertexSet[i]->adj) {
      int j = findVertexIdx(e.dest->info);
      W[i][j] = e.weight;
      P[i][j] = i;
    }
  }

  for (unsigned k = 0; k < n; k++)
    for (unsigned i = 0; i < n; i++)
      for (unsigned j = 0; j < n; j++) {
        if (W[i][k] == INF || W[k][j] == INF)
          continue; // avoid overflow
        int val = W[i][k] + W[k][j];
        if (val < W[i][j]) {
          W[i][j] = val;
          P[i][j] = P[k][j];
        }
      }
}

/*
template<class T>
vector<T> Graph<T>::getfloydWarshallPath(const T &orig, const T &dest) const{
        vector<T> res;
        int i = findVertexIdx(orig);
        int j = findVertexIdx(dest);
        if (i == -1 || j == -1 || W[i][j] == INF) // missing or disconnected
                return res;
        for ( ; j != -1; j = P[i][j])
                res.push_back(vertexSet[j]->info);
        reverse(res.begin(), res.end());
        return res;
}
*/
/**************** Minimum Spanning Tree  ***************/
/*
template <class T>
vector<Vertex<T>* > Graph<T>::calculatePrim() {
        // TODO
        for(unsigned int i = 0; i < vertexSet.size();i++){
                vertexSet[i]->dist = INF;
                vertexSet[i]->visited = false;
                vertexSet[i]->path = NULL;
        }

        vertexSet[0]->dist = 0;
        vertexSet[0]->visited = false;
        vertexSet[0]->path = NULL;

        MutablePriorityQueue<Vertex<T>> queue;

        queue.insert(vertexSet[0]);

        while(queue.empty() == false){
                Vertex<T> * v = queue.extractMin();
                v->visited = true;
                for(unsigned int i = 0; i < v->adj.size();i++){
                        if(v->adj[i].dest->visited == false){
                                if(v->adj[i].dest->dist == INF){
                                        v->adj[i].dest->dist = v->adj[i].weight;
                                        v->adj[i].dest->path = v;
                                        queue.insert(v->adj[i].dest);
                                }else{
                                        if(v->adj[i].dest->dist >
v->adj[i].weight){ v->adj[i].dest->dist = v->adj[i].weight; v->adj[i].dest->path
= v;
                                        }
                                        queue.decreaseKey(v->adj[i].dest);
                                }

                        }
                }
        }


        return vertexSet;
}
*/

template <class T> vector<Vertex<T> *> Graph<T>::calculateKruskal() {
  // TODO
  return getVertexSet();
}

#endif /* GRAPH_H_ */
