#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
using namespace std;

enum class Instructions: uint8_t {
  Branch = 0,
  Plus,
  Minus,
  Left,
  Right,
  In,
  Out,
};

/* Interpreter
 *    Contains state for a program -- the data array, instruction pointer,
 *    gas quantity, input string, and output string.
 ***********************************/ 
class Interpreter {
  uint8_t data[65536];
  uint32_t ptr;
  uint64_t gasCount; // Number of instructions for which the program may run
  bool gasActive;
  istringstream input;
  string output;
public:
  Interpreter(bool g, uint64_t ga, const string & in):
    gasActive(g),
    gasCount(ga),
    input(in),
    ptr(0),
    output("") {
    for (uint32_t i = 0; i < 65536; i++) data[i] = 0;
  };
  uint64_t gas();
  const string & getOutput() {return output;};
  uint8_t & cur();
  void advance();
  void retreat();
  void get();
  void put(uint8_t ch);
  void put();
};

// Reduce the gas count by one and return it. 
uint64_t Interpreter::gas() {
  gasCount = (gasActive ?
	      (gasCount ? gasCount - 1 : 0) :
	      1);
  return gasCount;
}

// Return a modifiable reference to the current data cell
uint8_t & Interpreter::cur() {
  return (data[ptr]); // add cell access logging?
}

// Move the instrunction pointer to the right
void Interpreter::advance() {
  ptr = (ptr + 1) % 65536;
}

// Move the instruction pointer to the left
void Interpreter::retreat() {
  ptr = (ptr == 0) ? 65535 : ptr - 1;
}

// Read a character from input to the current data cell
void Interpreter::get() {
  char c = 0;
  cur() = (uint8_t)(input.get(c) ? c : 0);
}

// Copy a byte to the output string
void Interpreter::put(uint8_t ch) {
  output.push_back((char) ch);
}

// Copy the current cell to the outpus string
void Interpreter::put() {
  put(cur());
}

/* Node
 *    A node in a tree structure which stores a parsed program
 *    and can execute it. It also has the parsing routine.
 *    Each node contains a list of instructions that are executed in the 
 *    current branch of the program, as well as a list of nodes that contain
 *    further branches. When a branch instruction is encountered, execution 
 *    passes to the corresponding node.
 ***********************************/ 
class Node {
  vector<Instructions> instructions;
  bool loop;
  vector<Node *> nodes;
public:
  Node(): loop(true) {};
  Node(bool lp): loop(lp) {}; 
  ~Node() {
    for (auto i = nodes.begin(); i != nodes.end(); i++) delete (*i);
  };
  void execute(Interpreter & in);
  void parse(istringstream & in);
  void print();
};

void Node::print() {
  for (auto i = instructions.begin(); i!= instructions.end(); i++)
    cout << (char)((uint8_t)(*i)+100) << " ";
  cout.flush();
  for (auto i = nodes.begin(); i!= nodes.end(); i++){
    cout << endl << ">>>    ";
    (*i)->print();
  }
}

// 
void Node::parse(istringstream & in) {
  Node * newClause = NULL;
  char c = 0;
  while (in.get(c)) {
    switch (c) {
    case '[':
      instructions.push_back(Instructions::Branch);
      newClause = new Node;
      newClause->parse(in);
      nodes.push_back(newClause);
      break;
    case '+':
      instructions.push_back(Instructions::Plus); break;
    case '-':
      instructions.push_back(Instructions::Minus); break;
    case '<':
      instructions.push_back(Instructions::Left); break;
    case '>':
      instructions.push_back(Instructions::Right); break;
    case ',':
      instructions.push_back(Instructions::In); break;
    case '.':
      instructions.push_back(Instructions::Out); break;
    case ']': // We are done parsing this branch
      return;
    }
  }
  return;
}

void Node::execute(Interpreter & in){
  do {
    auto idx = nodes.begin();
    for (auto i = instructions.begin();
	 i != instructions.end() && in.gas() != 0;
	 i++) {
      switch (*i){
      case Instructions::Branch:
	if (in.cur() != 0 &&  idx != nodes.end()) {
	  (*idx)->execute(in);
	}
	idx++;        break;
      case Instructions::Plus:
	in.cur()++;   break;
      case Instructions::Minus:
	in.cur()--;   break;
      case Instructions::Left:
	in.retreat(); break;
      case Instructions::Right:
	in.advance(); break;
      case Instructions::In:
	in.get();     break;
      case Instructions::Out:
	in.put();     break;
      }
    }
  } while  (in.cur() != 0 && loop && in.gas() != 0);
  return;
}

int main() {
  string program = "";
  string input = "";
  cout << "Program: ";
  cin >> program;
  cin.ignore(1000, '\n');
  cout << program << endl << "Input: ";
  cin >> input;
  cout << input << endl;
  cin.ignore(1000, '\n');
  Node root = Node(false);
  Interpreter in = Interpreter(true, 500, input);
  istringstream program_stream = istringstream(program);
  root.parse(program_stream);
  root.print();
  cout << endl;
  root.execute(in);
  cout << endl << in.getOutput() << endl;
  cout.flush();
  return 0;
}


extern "C" {
  const char * run (uint32_t gas, uint32_t gas_2, char * p, char * i) {
    string input = string(i);
    cout << p << endl << i << endl;
    istringstream program = istringstream(p);
    Node root = Node(false);
    root.parse(program);
    Interpreter in = Interpreter(gas != 0, gas*gas_2, input);
    root.execute(in);
    char * out = new char [in.getOutput().length() + 1];
    cout << in.getOutput() << endl;
    strcpy(out, in.getOutput().c_str());
    return out;
  }
}
