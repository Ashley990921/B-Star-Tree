#ifndef BSTAR_H
#define BSTAR_H

#include <vector>
#include <map>
#include <string>
#include "module.h"
using namespace std;

class Floorplanner
{
public:
    // constructor and destructor
    Floorplanner(double a){alpha=a;}
	~Floorplanner(){}
    // basic access methods
    
    
    
    void parse_blk(fstream& input);
    void parse_net(fstream& input);
    double HPWL() ;
    double calcost();
    double updcontour(Node* node);
    void coordinate(Node* node);
    void set_node();
    void delandins(int del, int ins);
    void swap(int node1, int node2);
    void SA();
    void writereport(double totaltime,fstream& output);
    double alpha;

    

private:
	Node*            Root = new Node("Root");
    vector<Node*>    node_list;
    vector<Block*>    blk_list;
    vector<Terminal*>  tml_list;
    vector<Net*>      net_list;
    map<string, Block*> name2blk;
    map<string, Terminal*> name2tml;
    map<string, Node*> name2node;
};
#endif 