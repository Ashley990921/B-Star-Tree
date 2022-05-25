#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <list>
#include <random>
#include "bstar.h"
using namespace std;

int chip_w, chip_h, num_blk, num_tml, num_net;
int x_max= 0;
int y_max= 0;

random_device rd;
default_random_engine gen(rd());


class contour_node {
public:
    contour_node(double x1, double x2, double y):
	    _x1(x1), _x2(x2), _y(y){ }
	double _x1, _x2, _y;

    ~contour_node() { }
};

list<contour_node> contour_line;


void Floorplanner::parse_blk(fstream& input){
    string trash, str;
	input >> trash; 
	input >> str;	
    chip_w = stod(str);
	
	input >> str;	
    chip_h = stod(str);
    
	input >> trash; 
	input >> str;	
    num_blk = stod(str);
	
	input >> trash; 
	input >> str;	
    num_tml = stod(str);
	
	for(int i=0; i<num_blk; ++i){
		string name;
		size_t w;
		size_t h;
		input >> str;	
        name = str;
		input >> str;  
        w=stod(str);
		input >> str;  
        h=stod(str);

		blk_list.push_back(new Block(name, w, h));
        if(name2blk.count(name) == 0) {
            name2blk[name] = blk_list[i];
        }
	}
	for(int i=0; i<num_tml; ++i){
		string name;
		size_t x;
		size_t y;
		input >> str;	
        name=str;
		input >> str;
		input >> str;	
        x= stod(str);
		input >> str;  
        y=stod(str);
		tml_list.push_back(new Terminal(name, x, y));
		if(name2tml.count(name) == 0) {
            name2tml[name] = tml_list[i];
        }
	}
	return;
}

void Floorplanner::parse_net(fstream& input){
    string str;
	input >> str;
	input >> str;	num_net = stod(str);
	for(int i=0; i<num_net; ++i){
		int eachNdegree;
		input >> str; 
		input >> str;	
        eachNdegree = stod(str);
        net_list.push_back(new Net());
	
		for(int j=0; j<eachNdegree; ++j){
			string name;
			input >> str;	
            name = str;
            net_list[i]->celllist.push_back(name); 
		}
	}

    ///cout<< net_list.size();
	return;
}

double Floorplanner::HPWL() {
    double totalLength = 0;
    for(int i=0;  i<net_list.size(); ++i) {
        double minx = 2000000000, rightx = 0;
        double miny = 2000000000, upy = 0;
        for(int j=0; j<net_list[i]->celllist.size(); j++) {
            string name=net_list[i]->celllist[j];
            ///cout<<name<<" ";
            if (name2blk.count(name)) {
                Block* curblock = name2blk[name];
                Node* curnode = name2node[name];
                ///cout<<curnode->_x<<" "<<curblock->_w<<endl;
                double macroX = double(curnode->_x) + double(curblock->_w)/double(2);
                double macroY = double(curnode->_y) + double(curblock->_h)/double(2);
                minx = min(minx,macroX);
                rightx = max(rightx,macroX);
                miny = min(miny,macroY);
                upy = max(upy,macroY);
            }
            else if(name2tml.count(name)) {
                Terminal* curTerminal = name2tml[name];
                double terminalX = curTerminal->_x;
                double terminalY = curTerminal->_y;
                minx = min(minx,terminalX);
                rightx = max(rightx,terminalX);
                miny = min(miny,terminalY);
                upy = max(upy,terminalY);
            }
            else{
                cout<<"no matching cell"<<endl;
            }
        }
        ///cout<<" "<<rightx<<" "<<minx<<" "<<upy<<" "<<miny<<endl;
        totalLength += (rightx-minx) + (upy-miny);
    }
    return totalLength;
}

double Floorplanner::calcost(){
    double x_error=max(x_max-chip_w, 0);
    double y_error=max(y_max-chip_h, 0);
    if(x_error+y_error==0){
        double area=x_max*y_max;
        double hpwl=HPWL();
        return -(chip_w*chip_h)*5/(alpha*area+(1-alpha)*hpwl);
    }
    else {
        return (x_error+y_error)*10;
    }
}


double Floorplanner::updcontour(Node* node){
    double y2, y1 = 0;
    double x1 = node->_x;
    double x2 = x1+name2blk[node->_name]->_w;
	list<contour_node>::iterator it = contour_line.begin();
	while(it != contour_line.end()){
		if(it->_x2 <= x1){
			++it;
		}
        else if(it->_x1 >= x2){
			break;
		}
        else{
			y1 = max(it->_y, y1);

			if(it->_x1>=x1 && it->_x2<=x2){
				it=contour_line.erase(it);
			}
            else if( it->_x1>=x1 && it->_x2>=x2 ){
				it->_x1=x2;
                continue;
			}
            else if( it->_x1<=x1 && it->_x2<=x2 ){
				it->_x2=x1;
                continue;
			}
            else{
				contour_line.insert(it,contour_node(it->_x1,x1,it->_y));
				it->_x1=x2;
                continue;
			}
		}
	}
	y2 = y1+name2blk[node->_name]->_h;
	contour_line.insert(it,contour_node(x1,x2,y2));
    if(y2 > y_max) y_max = y2;
    
	return y1;
}



void Floorplanner::coordinate(Node* node){
    if(node!=NULL){
        size_t caly=0;
        if(node->_prev==NULL){
            node->_x=0;
            node->_y=0;      
            contour_line.push_back(contour_node(0, name2blk[node->_name]->_w, name2blk[node->_name]->_h));
            y_max=name2blk[node->_name]->_h;
            x_max=name2blk[node->_name]->_w;
        }
        else if(node==node->_prev->_left){
            double x = node->_prev->_x+name2blk[node->_prev->_name]->_w;
		    node->_x=x;
            if(x+name2blk[node->_name]->_w > x_max){
                x_max = x+name2blk[node->_name]->_w;
            }
            caly=updcontour(node);
		    node->_y=caly;
        }
        else if(node==node->_prev->_right){
            double x = node->_prev->_x;
		    node->_x=x;
            if(x+name2blk[node->_name]->_w > x_max){
                x_max = x+name2blk[node->_name]->_w;
            }
            caly=updcontour(node);
		    node->_y=caly;
        }       
        coordinate(node->_left);
        coordinate(node->_right);
        
    } 
}

void Floorplanner::set_node(){
	for(int i=0; i<num_blk; ++i){
		string name = blk_list[i]->_name;
		node_list.push_back(new Node(name));
		if(name2node.count(name) == 0) name2node[name] = node_list[i];
	}
	
	Root=node_list[0];
    Root->_left=node_list[1];
    node_list[1]->_prev=Root;

    for(int i=1; i<num_blk-1; i++){
        node_list[i]->_left=node_list[i+1];
        node_list[i+1]->_prev=node_list[i];
    }

    coordinate(Root);  ///set coordinate and calculate xmax, ymax
	return;
}

void Floorplanner::delandins(int del, int ins){
    ////delete
    if(node_list[del]->_left != NULL){
		node_list[del]->_left->_prev = node_list[del]->_prev;
		if(node_list[del]->_prev->_left == node_list[del]) node_list[del]->_prev->_left = node_list[del]->_left;
		else node_list[del]->_prev->_right = node_list[del]->_left;
		node_list[del]->_left = NULL;
	}
    else if(node_list[del]->_right != NULL){
		node_list[del]->_right->_prev = node_list[del]->_prev;
		if(node_list[del]->_prev->_left == node_list[del]) node_list[del]->_prev->_left = node_list[del]->_right;
		else node_list[del]->_prev->_right = node_list[del]->_right;
		node_list[del]->_right = NULL;
	}
    else{
		if(node_list[del]->_prev->_left == node_list[del]) node_list[del]->_prev->_left = NULL;
		else node_list[del]->_prev->_right = NULL;
	}

    //// insert
		if(node_list[ins]->_left == NULL && node_list[ins]->_right == NULL){
			if(rand()&1) node_list[ins]->_left = node_list[del];
			else node_list[ins] ->_right = node_list[del];
		}
        else if(node_list[ins]->_left == NULL){
			node_list[ins]->_left = node_list[del];
		}
        else if(node_list[ins]->_right == NULL){
			node_list[ins]->_right = node_list[del];
        }
		node_list[del]->_prev = node_list[ins];
}

void Floorplanner::swap(int node1, int node2){
    Block *blk1=name2blk[node_list[node1]->_name];
    Block *blk2=name2blk[node_list[node2]->_name];
    name2blk[blk1->_name]=blk2;
    name2blk[blk2->_name]=blk1;
    name2node[blk1->_name]=node_list[node2];
    name2node[blk2->_name]=node_list[node1];
    string t_name=blk1->_name;
    size_t t_w=blk1->_w;         // width of the block
    size_t t_h=blk1->_h;
    blk1->_name=blk2->_name;
    blk1->_w=blk2->_w;
    blk1->_h=blk2->_h;
    blk2->_name=t_name;
    blk2->_w=t_w;
    blk2->_h=t_h;

    ///string tn_name=node_list[node1]->_name;
    node_list[node1]->_name=node_list[node2]->_name;
    node_list[node2]->_name=t_name;
}




void Floorplanner::SA(){
    int reject=0;
    double T=100000;
    uniform_real_distribution<> disexp(0.0,1.0);
    while(1){
        reject = 0;
        for(int i=0; i<1000; ++i){
            x_max = 0;
	        y_max = 0;
            contour_line.clear();
            coordinate(Root);
            double cost=calcost();
            uniform_int_distribution<> dis(1,3);
            int choice = dis(gen);
            if(choice==1){
                uniform_int_distribution<> dis(0,num_blk-1);
                int choice_rot_blk = dis(gen);
                size_t temp;
                temp = blk_list[choice_rot_blk]->_w;
                blk_list[choice_rot_blk]->_w=blk_list[choice_rot_blk]->_h;
                blk_list[choice_rot_blk]->_h=temp;

                

                x_max = 0;
	            y_max = 0;
                contour_line.clear();
                coordinate(Root);
				double cost2 = calcost();

                double prob2 =disexp(gen);
				if(cost < cost2 && prob2 >=exp((double)(cost-cost2)/T) ){
					reject++;
                    size_t temp;
                    temp = blk_list[choice_rot_blk]->_w;
                    blk_list[choice_rot_blk]->_w=blk_list[choice_rot_blk]->_h;
                    blk_list[choice_rot_blk]->_h=temp;
				}
            }
            if(choice==2){
                int del_node;
                int ins_node;
                while(true){
                    uniform_int_distribution<> dis(1,num_blk-1);////from 1 because not choosing root
                    uniform_int_distribution<> dis2(0,num_blk-1);
                    del_node=dis(gen);
                    ins_node=dis2(gen);
                    if((node_list[del_node] != node_list[ins_node])&&(node_list[ins_node]->_left == NULL || node_list[ins_node] ->_right == NULL)&&(node_list[del_node]->_left == NULL || node_list[del_node]->_right == NULL)){
                                break;     
                    }
                }


                bool prev_lorr;///0 for left, 1 for right
                Node *prevblk=node_list[del_node] ->_prev;
                if(node_list[del_node]->_prev->_left==node_list[del_node]) prev_lorr=0;
                else prev_lorr=1;

                int child_lorr;///0 for left, 1 for right, 2 for none
                Node *childblk;
                if(node_list[del_node]->_left!=NULL) {
                    childblk=node_list[del_node]->_left;
                    child_lorr=0;
                }
                else if(node_list[del_node]->_right!=NULL) {
                    childblk=node_list[del_node]->_right;
                    child_lorr=1;
                }
                else child_lorr=2;
                
                delandins(del_node, ins_node);

  
                x_max = 0;
	            y_max = 0;
                contour_line.clear();
                coordinate(Root);
				double cost2 = calcost();

                
                double prob2 = disexp(gen);
				if(cost < cost2 && prob2 >=exp((double)(cost-cost2)/T)){
                    reject++;
					if(node_list[ins_node]->_left == node_list[del_node])  node_list[ins_node]->_left=NULL;
                    else node_list[ins_node]->_right=NULL;

                    if(prev_lorr==0) {
                        if(child_lorr==0){
                            node_list[del_node]->_left=childblk;
                            childblk->_prev=node_list[del_node];
                        }
                        else if(child_lorr==1){
                            node_list[del_node]->_right=childblk;
                            childblk->_prev=node_list[del_node];
                        }
                        prevblk->_left=node_list[del_node];
                        node_list[del_node]->_prev=prevblk;
                    }
                    else if(prev_lorr==1) {
                        if(child_lorr==0){
                            node_list[del_node]->_left=childblk;
                            childblk->_prev=node_list[del_node];
                        }
                        else if(child_lorr==1){
                            node_list[del_node]->_right=childblk;
                            childblk->_prev=node_list[del_node];
                        }
                        prevblk->_right=node_list[del_node];
                        node_list[del_node]->_prev=prevblk;
                    }

				}
                

            }
            if(choice==3){
                uniform_int_distribution<> dis(0,num_blk-1);
                uniform_int_distribution<> dis2(0,num_blk-1);
                int fir_node=dis(gen);
                int sec_node=dis2(gen);
                while(fir_node==sec_node){
                    uniform_int_distribution<> dis3(0,num_blk-1);
                    sec_node=dis3(gen);
                }
                swap(fir_node, sec_node);
                x_max = 0;
	            y_max = 0;
                contour_line.clear();
                coordinate(Root);
				double cost2 = calcost();

                
                double prob2 = disexp(gen);
				if(cost < cost2 && prob2 >=exp((double)(cost-cost2)/T)){
                    reject++;
                    swap(fir_node, sec_node);
                }
            }
        }

        T*=0.6;
        double rejectRate = (double)reject/1000;
        if((rejectRate > 0.96) && (x_max-chip_w<=0) && (y_max-chip_h<=0)) {
            ///cout<<"here";
            return;
        }
        if(T < 0.000001 && (x_max-chip_w<=0) && (y_max-chip_h<=0) ) return;
    }

    return;
}

void Floorplanner::writereport(double totaltime,fstream& output){
    x_max = 0;
	y_max = 0;
    contour_line.clear();
    coordinate(Root);

    double wirelength=HPWL();
    double area=x_max*y_max;
    output<<fixed<<alpha*area+(1.0-alpha)*wirelength<<endl;
    output<<fixed<<wirelength<<endl;
    output<<area<<endl;
    output<<x_max<<" "<<y_max<<endl;
    output<<totaltime<<endl;
    for(int i=0; i<num_blk; i++){
        Node* node=name2node[blk_list[i]->_name];
        output<<blk_list[i]->_name<<" "<<node->_x<<" "<<node->_y<<" "<<node->_x+blk_list[i]->_w<<" "<<node->_y+blk_list[i]->_h<<endl;
    }
}
