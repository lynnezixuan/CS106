/* This is th cpp file for searching paths through graphs representing maps,
 * mazes, and rocky terrains.It demonstrates several graph algorithms for
 * finding paths, such as depth-first search (DFS),breadth-first search (BFS),
 * Dijkstra's Algorithm,and A* search. Besides, it can also generate random mazes
 * using Kruskal's algorithm.
 *
 * by Zixuan Zhou
 */

#include "trailblazer.h"
#include "queue.h"
#include "priorityqueue.h"
using namespace std;
/* set a certain color for all vertex in the vector.
 */
void colorVertex( Vector<Vertex*> vt_vector,int color ){
    for( Vertex* current : vt_vector) {
         current->setColor(color);
    }
}
/* DPS for finding path, recursive function, refering to the code in lecture 10
 * arg: graph, start vertex, end vertex
 * output: vector of vertice on the path
 */
bool depthFirstSearchHelper(BasicGraph& graph, Vector<Vertex*> &path, Vertex* start, Vertex* end){
    //base case: if current start vertex is the desitination, then end searching
    if ( start == end )
        return true;
    //recursive case: if current start vertex is not the end, continue searching
    else{
        Set<Vertex*> neighbor =  graph.getNeighbors(start);// get set of current start vertex's neighbors
        for ( Vertex* current : neighbor ){// current neigbor
            if(current->visited)//if the current neigbor has been visited, then skip it.
                continue;
            path.add(current);//add the vertex into the path
            current->visited = true;//mark the vertex
            if(depthFirstSearchHelper( graph, path, current, end))//if find a path, return ture directly
                return true;
            current->setColor(GRAY);//mark impossible vertex for the final path
            path.remove(path.size()-1);// remove the impossible vertex from the path

        }
        return false;// cannot find any path
    }
}

/* DPS for finding path, wrapper function
 * arg: graph, start vertex, end vertex
 * return: vector of vertice on the path
 */
Vector<Vertex*> depthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    Vector<Vertex*> path;
    graph.resetData();//make sure that no stale data is left in the vertexes
    path.add(start);//add start vertex into the path
    start->visited = true;//mark start
    depthFirstSearchHelper(graph, path, start, end);
    colorVertex(path,GREEN);//color the vertex on the path
    return path;
}
/* BPS for finding path, refering to the code in lecture 23
 * arg: graph, start vertex, end vertex
 * output: vector of vertice on the path
 */
bool breadthFirstSearchHelper(BasicGraph& graph, Vertex* start, Vertex* end){
    Queue<Vertex*> toExplore;// store vertice which are to be explored
    toExplore.enqueue(start);// store start vertex
    start->setColor(YELLOW);//when enqueue a vertex, color it yellow
    //if we have not find the end vertex and there are vertice to explore in the queue, then continue
    while(!toExplore.isEmpty()){
        Vertex* current = toExplore.dequeue();
        current->setColor(GREEN);//when a vertex is dequeued, then color it green
        if (current == end)//if find the end, then return directly
            return true;
        if (current->visited)//if the vertex has been visited, skip it
            continue;
        current->visited = true;//mark the vertex
        Set<Vertex*> neighbor_set = graph.getNeighbors(current);
        for (Vertex* neighbor : neighbor_set){
            if(neighbor->getColor() == UNCOLORED){//we would only deal with those uncolored neigbors in BFS
                toExplore.enqueue(neighbor);
                neighbor->previous = current;//for backtracking the path
                neighbor->setColor(YELLOW);
            }
        }
    }
    return false;
}
/* according to the vertex->previous
 * backtracking the final path
 */
void findPath(Vertex* current,Vector<Vertex*>& path ){
    //base case:backtracking and find the start vertex
    if (current == NULL)
        return;
    //recursive case
    else{
        findPath(current->previous, path);
        path.add(current);//add current vertex into the path
    }
}
/* BPS for finding path, wrapper function
 * arg: graph, start vertex, end vertex
 * return: vector of vertice on the path
 */
Vector<Vertex*> breadthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    Vector<Vertex*> path;
    graph.resetData();
    breadthFirstSearchHelper(graph, start, end);
    findPath(end, path);// find the final path
    return path;
}
/* Dijstras and A star for finding weighted path, refering to the pseudo code in lecture 24,25
 * arg: graph, start vertex, end vertex, if the flag of isHeuristic = 0 for Dijstras (default)
 * and isHeuristic = 1 for A star
 *
 * output: vector of vertice on the path
 */
bool DijkstrasAStarHelper(BasicGraph& graph, Vertex* start, Vertex* end, bool isHeuristic = 0 ) {
    PriorityQueue<Vertex*> toExplore;// store vertice which are to be explored
    start->setColor(YELLOW);//when enqueue a vertex, color it with yellow
    start->cost = 0;
    toExplore.enqueue(start, 0);//store start vertex
    //if we have not find the end vertex and there are vertice to explore in the queue, then continue
    while(!toExplore.isEmpty()){
        Vertex* current = toExplore.dequeue();
        current->setColor(GREEN);//when a vertex is dequeued, then color it with green
        if (current == end)//if find the end, then return directly
            return true;
        Set<Vertex*> neighbor_set = graph.getNeighbors(current);
        for ( Vertex* neighbor : neighbor_set){
            if(neighbor->getColor() == GREEN)//"Green" means that vertex has been done
                continue;
            Edge* current_edge = graph.getEdge(neighbor, current);//find the edge to get the traversal cost/weight
            double admiss_heur = isHeuristic * heuristicFunction(neighbor, end);//admissible heuristic
            double current_cost = current->cost + current_edge->cost;//d+L
            bool isExplore = (neighbor->getColor() == UNCOLORED)//if neighbor of current vertex is uncolored, enqueue it
                    //if neighbor of current vertex is yellow, and get a more ergent priority, replace with new priority in the queue
                    ||(neighbor->getColor() == YELLOW && neighbor->cost > current_cost);
            if(isExplore){
                neighbor->cost = current_cost;
                if(neighbor->getColor() == UNCOLORED)//for A star, priority will count the admissible heuristic
                    toExplore.enqueue(neighbor, neighbor->cost + admiss_heur);
                else
                    toExplore.changePriority(neighbor, neighbor->cost + admiss_heur);
                neighbor->previous = current;//for backtracking the path
                neighbor->setColor(YELLOW);
            }
        }
    }
    return false;
}
/* Dijstras for finding path, wrapper function
 * arg: graph, start vertex, end vertex
 * return: vector of vertice on the path
 */
Vector<Vertex*> dijkstrasAlgorithm(BasicGraph& graph, Vertex* start, Vertex* end) {
    Vector<Vertex*> path;
    graph.resetData();
    DijkstrasAStarHelper(graph, start, end);
    findPath(end, path);//find the final path
    return path;
}
/* A star for finding path, wrapper function
 * arg: graph, start vertex, end vertex
 * return: vector of vertice on the path
 */
Vector<Vertex*> aStar(BasicGraph& graph, Vertex* start, Vertex* end) {
    Vector<Vertex*> path;
    graph.resetData();
    DijkstrasAStarHelper(graph, start, end, true);
    findPath(end, path);//find the final path
    return path;
}
/* use the very start/parent vertex of a directed edge to represent cluster
 * ex. A->B->C->D, thus A, B, C, D would be considered in the cluster A
 * recursive function
 */
Vertex* findCluster( Vertex* current, const Map< Vertex*, Vertex* >& cluster ){
    //base case: find the very start vertex
    //for the very start vertex, the name of cluster for it will be its own name
    if (current == cluster[current])
        return current;
    //recursive case
    else
        return findCluster(cluster[current], cluster);
}
/* Kruskal's algorithm for finding a minimum spanning tree
 * arg: graph
 * return: a set of pointers to edges in the graph such that those edges would
 * connect the graph's vertexes into a minimum spanning tree.
 */
Set<Edge*> kruskal(BasicGraph& graph) {
    Set<Edge*> mst;//a set of pointers to edges which would connect the graph's vertexes into a MST
    Map<Vertex*, Vertex*> cluster;// record the vertex and their cluster
    PriorityQueue<Edge*> toExplore;// store vertice which are to be explored
    graph.resetData();
    int vertex_num = graph.size();
    int added_vertex = 1;
    Set<Vertex*> vertex_set = graph.getVertexSet();
    Set<Edge*> edge_set = graph.getEdgeSet();
    // intialieze clusters/parent vertex for each vertice in the graph, it would be itself
    for (Vertex* current_vt : vertex_set){
        cluster[current_vt] = current_vt;
    }
    //enqueue all edge into priority queue
    for (Edge* current_edge : edge_set){
        toExplore.enqueue(current_edge,current_edge->cost);
    }
    //loop until we connect all vertice in the graph
    while(added_vertex < vertex_num && !toExplore.isEmpty()){
        Edge* current_edge = toExplore.dequeue();
        Vertex* start = current_edge->start;
        Vertex* finish = current_edge->finish;
        //if the start vertex and finish vertex of current edge are in same cluster/ have same parent vertex, we will get rid of it
        bool isConnected = findCluster(start, cluster) == findCluster(finish, cluster);
        if(!isConnected){
            mst.add(current_edge);
            /* add current edge (add the finish vertex into the start vertex's cluster)
             * i.e. let the parent of finish vertex becomes the chird of start's parent.
             */
            Vertex* current_cluster = findCluster(finish, cluster);
            cluster[current_cluster] = findCluster(start, cluster);
        }
    }

    return mst;
}
