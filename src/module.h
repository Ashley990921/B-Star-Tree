#ifndef MODULE_H
#define MODULE_H

#include <vector>
using namespace std;

class Block{
public:
    // constructor and destructor
    Block(string& name, size_t w, size_t h) :
        _name(name), _w(w), _h(h) { }
    ~Block() { }

    string          _name;
    size_t          _w;         // width of the block
    size_t          _h;         // height of the block
    bool            rotate=false;
    const size_t getArea()  { return _h * _w; }


    // set functions
    void setName(size_t name)         { _name = name; }
    void setWidth(size_t w)         { _w = w; }
    void setHeight(size_t h)        { _h = h; }
    void setRotate()      {rotate = !(rotate); }

};

class Node
{
public:
	// constructor and destructor
	Node(string name):_name(name), _prev(NULL), _right(NULL), _left(NULL) {	}
	~Node() {	}
	
	// basic access methods
	string    _name;
	Node*    _prev;
	Node*    _right;
	Node*    _left;
	size_t    _x;
	size_t    _y;
	
	// set functions
    void setName(string s) {_name = s; }
 	void setPrev(Node* Prev) {_prev = Prev; }
	void setRight(Node* Right) {_right = Right; }
	void setLeft(Node* Left) {_left = Left; } 
	void setX(size_t x)	{ _x = x; }
	void setY(size_t y) { _y = y; }
	
};

class Terminal
{
public:
    // constructor and destructor
    Terminal(string& name, size_t x, size_t y) :
        _name(name), _x(x), _y(y) { }
    ~Terminal()  { }

    // basic access methods
    string      _name;      // module name
    size_t      _x;        // min x coordinate of the terminal
    size_t      _y;


    // set functions
    void setName(string& name) { _name = name; }
    void setPos(size_t x, size_t y) {
        _x = x;   _y = y;
    }

};

class Net
{
public:
    // constructor and destructor
    Net()   { }
    ~Net()  { }

    // basic access methods
    vector<string>   celllist;

};

#endif 