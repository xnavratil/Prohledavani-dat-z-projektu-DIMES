/**
 * @file    main.c
 *
 * You can modify this file as you desire.
 *
 * If you want to document it Doxygen, the first two lines must remain as
 * they are now. The comment tells Doxygen that this file should be
 * processed.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "graph.h"
#include "heap.h"

/**
 * @brief loadNodes loading nodes from F
 * @param graph structure for adding nodes
 * @param f file for reading
 * @return true if successful false if not
 */

bool loadNodes(Graph *graph, FILE* f){
    unsigned int id = 0;
    char buffer[201];
    while(fgets(buffer,200,f)){
        id = atoi(strtok(buffer,","));
        if(!graph_insert_node(graph,id)){
            fputs("nedostatok pamati pri nacitavani vrcholov\n",stderr);
            return false;
        }
    }
    if(id == 0){
        fputs("ziadne vrcholy v grafe\n",stderr);
        return false;
    }
    return true;
}
/**
 * @brief loadEdges loading edges from file F
 * @param graph structure for adding edges
 * @param f file for reading
 * @return true if successful false if not
 */
bool loadEdges(Graph *graph, FILE* f){
    unsigned int source = 0;
    unsigned int destination = 0;
    int mindelay = 0;
    char buffer[201];
    while(fgets(buffer,200,f)){
        source = atoi(strtok(buffer,","));
        destination = atoi(strtok(NULL,","));
        strtok(NULL,",");
        mindelay = atoi(strtok(NULL,","));
        if(!graph_insert_edge(graph,source,destination,mindelay)){
            fputs("nedostatok pamati pri nacitavani hran\n",stderr);
            return false;
        }
    }
    return true;
}
/**
 * @brief dijkstra function for exploring graph
 * @param graph structure to be explored
 * @param s starting node
 * @param d destination node
 */
void dijkstra(Graph* graph, Node* s, Node* d){
    Heap *heap = heap_new_from_graph(graph);
    if (!heap){
        fputs("nedostatok pamati pre haldu\n",stderr);
        return;
    }
    Node * current = NULL;
    struct edge * edges = NULL;
    unsigned short out = 0;
    unsigned short alt = 0;
    heap_decrease_distance(heap,s,0,NULL);
    while(!heap_is_empty(heap)){
        current = heap_extract_min(heap);
        if(node_get_distance(current) == UINT_MAX){
            break;
        }
        if(current == d){
            break;
        }
        edges = node_get_edges(current);
        out = node_get_n_outgoing(current);
        for(int i = 0; i < out; i++){
            alt = node_get_distance(current) + edges[i].mindelay;
            if(alt < node_get_distance(edges[i].destination)){
                heap_decrease_distance(heap,edges[i].destination,alt,current);
            }
        }
    }
    heap_free(heap);
}
int main(int argc, char* argv[]){
    FILE *fNodes = NULL;
    FILE *fEdges = NULL;
    if((argc < 5)||(argc > 6)){
        fputs("zly pocet argumentov\n",stderr);
        return 1;
    }
    Graph * graph = graph_new();
    if(!graph){
        fputs("nepodarilo sa vytvorit graf / malo pamate\n",stderr);
        return 2;
    }
    fNodes = fopen(argv[1],"r");
    if(!fNodes){
        graph_free(graph);
        fputs("zadany subor vrcholov neexistuje\n",stderr);
        return 3;
    }
    fEdges = fopen(argv[2],"r");
    if(!fEdges){
        fclose(fNodes);
        graph_free(graph);
        fputs("zadany subor hran neexistuje\n",stderr);
        return 3;
    }
    if(!loadNodes(graph, fNodes)){
        fclose(fNodes);
        fclose(fEdges);
        graph_free(graph);
        return 4;
    }
    if(!loadEdges(graph, fEdges)){
        fclose(fNodes);
        fclose(fEdges);
        graph_free(graph);
        return 4;
    }
    fclose(fNodes);
    fclose(fEdges);
    unsigned int source = atoi(argv[3]);
    unsigned int destination= atoi(argv[4]);
    Node * s = graph_get_node(graph,source);
    if(!s){
        fputs("neexistuje vychodzi bod\n",stderr);
        graph_free(graph);
        return 5;
    }
    Node * d = graph_get_node(graph,destination);
    if(!d){
        fputs("neexistuje cielovy bod\n",stderr);
        graph_free(graph);
        return 5;
    }
    if(s != d){
        dijkstra(graph,s,d);
        if(node_get_previous(d) == NULL){
            fputs("cesta neexistuje\n",stderr);
            graph_free(graph);
            return 6;
        }
    }else{
        if(argc == 5){
            fprintf(stdout,"digraph {\n");
            fprintf(stdout,"}\n");
            graph_free(graph);
            return 0;
        }else{
            FILE * result = fopen(argv[5], "w");
            if(!result){
                graph_free(graph);
                fputs("nepodarilo sa otvorit subor na vypis\n",stderr);
                return 7;
            }
            fprintf(result,"digraph {\n");
            fprintf(result,"}\n");
            fclose(result);
            graph_free(graph);
            return 0;
        }
    }
    if(argc == 5){
        fprintf(stdout,"digraph {\n");
        while(d != s){
            fprintf(stdout,"\t%u -> %u [label=%u];\n", node_get_id(node_get_previous(d)),node_get_id(d),(node_get_distance(d) - node_get_distance(node_get_previous(d))));
            d = node_get_previous(d);
        }
        fprintf(stdout,"}\n");
    }else{
        FILE * result = fopen(argv[5], "w");
        if(!result){
            graph_free(graph);
            fputs("nepodarilo sa otvorit subor na vypis\n",stderr);
            return 7;
        }
        fprintf(result,"digraph {\n");
        while(d != s){
            fprintf(result,"\t%u -> %u [label=%u];\n", node_get_id(node_get_previous(d)),node_get_id(d),(node_get_distance(d) - node_get_distance(node_get_previous(d))));
            d = node_get_previous(d);
        }
        fprintf(result,"}\n");
        fclose(result);
    }
    graph_free(graph);
    return 0;
}
