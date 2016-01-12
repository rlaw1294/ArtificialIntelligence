/* Author: Rachel Law
	ID 861071722
	rlaw001@ucr.edu
	11/5/15

	Project 1
	CS 170: Introduction to Artificial Intelligence
	Instructor: Dr. Eamonn Keogh
*/

#include <iostream>
#include <vector>
#include <queue>
#include <math.h>
#include <string>
#include <ctime>
using namespace std;

class Node {
public:
	/* CLASS VARIABLES */
	vector<vector<int> > board;
	vector<vector<int> > *goal_board;
	/*
		i0 i1 i2		(i is cols which is inside)
	j0	[1  2  3]		(j is rows which is outside wrapping of vector)
	j1  [4  5  6]
	j2  [7  8  0]		*/

	int cost_g;	// Cost of path so far
	int cost_h; // Cost of heuristic

	Node* left;
	Node* right;
	Node* down;
	Node* up;
	Node* parent;

	int index_blank_row;
	int index_blank_col;

	/* CONSTRUCTORS */
	// Constructor for making the root node
	Node(const vector<vector<int> > &start_state, const vector<vector<int> > &goal_state) {
		board = start_state;
		vector<vector<int> > *temp = new vector<vector<int> >;
		*temp = goal_state;
		goal_board = temp;
		cost_g = 0;
		cost_h = 0;

		left, right, down, up = NULL;
		parent = NULL;

		setBlankIndexes();
	}

	// Constructor for generating nodes' children / generates the "fringe"
	Node(Node* parent, char dir, string heuristic) {
		board = parent->board;
		goal_board = parent->goal_board;
		cost_g = parent->cost_g + 1;

		if (heuristic == "Uniform_Cost_Search") cost_h = 0;							// Uniform_Cost_Search
		else if (heuristic == "Misplaced_Tiles") { cost_h = misplacedTiles(); }		// Misplaced_Tiles
		else { cost_h = manhattanDistance(); }										// Manhattan

		left, right, down, up = NULL;
		this->parent = parent;

		switch (dir)
		{
		case 'l':
			index_blank_row = parent->index_blank_row;
			index_blank_col = parent->index_blank_col - 1;
			swapBoardValue(parent->index_blank_row, parent->index_blank_col, index_blank_row, index_blank_col);
			break;
		case 'r':
			index_blank_row = parent->index_blank_row;
			index_blank_col = parent->index_blank_col + 1;
			swapBoardValue(parent->index_blank_row, parent->index_blank_col, index_blank_row, index_blank_col);
			break;
		case 'd':
			index_blank_row = parent->index_blank_row + 1;
			index_blank_col = parent->index_blank_col;
			swapBoardValue(parent->index_blank_row, parent->index_blank_col, index_blank_row, index_blank_col);
			break;
		case 'u':
			index_blank_row = parent->index_blank_row - 1;
			index_blank_col = parent->index_blank_col;
			swapBoardValue(parent->index_blank_row, parent->index_blank_col, index_blank_row, index_blank_col);
			break;
		}
	}


	/* HEURISTICS */
	int misplacedTiles() {
		int count = 0;
		for (unsigned int row = 0; row < board.size(); row++) {
			for (unsigned int col = 0; col < board.at(row).size(); col++) {
				if (board.at(row).at(col) != (*goal_board).at(row).at(col)) count++;
			}
		}
		return count;
	}

	/* Finds the Manhattan Distance
		(1) Find and store index locations for each number for board and goal board
		(2) Get the distance each number would have to move from board's index to get to the goal board's index
			adding absolute values of the differences in index locations
	*/
	int manhattanDistance() {
		vector<int> row_index_board(board.size()*board.at(0).size(), 0);
		vector<int> col_index_board(board.size()*board.at(0).size(), 0);

		vector<int> row_index_goalboard(board.size()*board.at(0).size(), 0);
		vector<int> col_index_goalboard(board.size()*board.at(0).size(), 0);

		for (unsigned row = 0; row < board.size(); row++) {
			for (unsigned col = 0; col < board.at(row).size(); col++) {
				int board_num = board.at(row).at(col);
				row_index_board[board_num] = row;
				col_index_board[board_num] = col;
			}
		}
		for (unsigned row = 0; row < (*goal_board).size(); row++) {
			for (unsigned col = 0; col < (*goal_board).at(row).size(); col++) {
				int goal_board_num = (*goal_board).at(row).at(col);
				row_index_goalboard[goal_board_num] = row;
				col_index_goalboard[goal_board_num] = col;
			}
		}

		int distance = 0;
		for (unsigned int i = 1; i < row_index_board.size(); i++) {
			distance += abs(row_index_board[i] - row_index_goalboard[i]);
		}
		for (unsigned int i = 1; i < col_index_board.size(); i++) {
			distance += abs(col_index_board[i] - col_index_goalboard[i]);
		}

		return distance;
	}


	/* HELPER FUNCTIONS */
	// Finds the indices of the blank in the Node's board
	void setBlankIndexes() {
		for (unsigned int row = 0; row < board.size(); row++) {
			for (unsigned int col = 0; col < board.at(row).size(); col++) {
				if (board.at(row).at(col) == 0) {
					index_blank_row = row;
					index_blank_col = col;
					return;
				}
			}
		}
	}

	// Swaps the two values at index_a and index_b on a Node's board. Used to 'move' the blank.
	void swapBoardValue(int index_a_row, int index_a_col, int index_b_row, int index_b_col) {
		int temp = board.at(index_a_row).at(index_a_col);
		board.at(index_a_row).at(index_a_col) = board.at(index_b_row).at(index_b_col);
		board.at(index_b_row).at(index_b_col) = temp;
	}

	// Creates the Node's children / Creates the "fringe"
	void makeChildren(string heuristic) {
		if (index_blank_col > 0) {												// left => index_blank_col - 1
			Node* left_child = new Node(this, 'l', heuristic);
			left = left_child;
		}
		if (index_blank_col < (int)board.at(index_blank_row).size() - 1) {		// right => index_blank_col + 1
			Node* right_child = new Node(this, 'r', heuristic);
			right = right_child;
		}
		if (index_blank_row < (int)board.at(index_blank_row).size() - 1) {		// down => index_blank_row + 1
			Node* down_child = new Node(this, 'd', heuristic);
			down = down_child;
		}
		if (index_blank_row > 0) {												// up => index_blank_row - 1
			Node* up_child = new Node(this, 'u', heuristic);
			up = up_child;
		}
	}


	// Print out the Node's board
	void printBoard() {
		for (unsigned int row = 0; row < board.size(); row++) {
			for (unsigned int col = 0; col < board.at(row).size(); col++) {
				cout << board.at(row).at(col) << "\t";
			}
			cout << endl;
		}
	}

	// Returns TRUE if the passed in Node has a matching board
	bool isMatchingBoard(Node* A) {
		for (unsigned int row = 0; row < board.size(); row++) {
			for (unsigned int col = 0; col < board.at(row).size(); col++) {
				if (board.at(row).at(col) != A->board.at(row).at(col)) return false;
			}
		}
		return true;
	}

	// Returns TRUE if the Node's board matches the Node's goal_board
	bool isMatchingGoalBoard() {
		for (unsigned int row = 0; row < board.size(); row++) {
			for (unsigned int col = 0; col < board.at(row).size(); col++) {
				if (board.at(row).at(col) != (*goal_board).at(row).at(col)) return false;
			}
		}
		return true;
	}

	// Returns TRUE if this Node's board matches a board state in the 'checked' vector
	bool alreadyCheckedBoard(const vector<Node*> &checked) {
		for (unsigned int i = 0; i < checked.size(); i++) {
			if (isMatchingBoard(checked.at(i))) return true;
		}
		return false;
	}

	// Class needed to create a class comparison funtion in order to figure out which node has a lower path cost for our priority queue
	class ComparePriority {
	public:
		bool operator()(Node *A, Node *B) {// Returns true if A cost more than B
			return A->cost_g + A->cost_h > B->cost_g + B->cost_h;
		}
	};

	// Creates the tree and traverses the tree until solved. Returns a pointer to the solved Node
	Node* Solver(string heuristic) {
		Node* current = NULL;

		// Node Counters
		int nodes_expanded = 0;
		unsigned max_nodes_in_queue = 1;

		// Priority Queue
		priority_queue<Node*, vector<Node*>, ComparePriority > pq;
		pq.push(this);

		// Hold Node* to check alredy seen board states
		vector<Node*> checked;

		// Timer
		clock_t start_clock, end_clock;


		start_clock = clock();
		while (!pq.empty()) {
			current = pq.top();
			//cout << "Step g(n) + Heuristic h(n): " << current->cost_g << " + " << current->cost_h << " = " << (current->cost_g + current->cost_h) << endl;
			pq.pop();
			nodes_expanded++; // Every time I pop off a node, I expand it
			checked.push_back(current);

			if (current->isMatchingGoalBoard()) {
				end_clock = clock();
				//cout << "Solution found in " << end_clock - start_clock << " clock ticks " << endl;
				cout << "Number nodes expanded: " << nodes_expanded << endl;
				cout << "Max number nodes in queue: " << max_nodes_in_queue << endl;
				return current;
			}

			current->makeChildren(heuristic);
			if (current->left && !current->left->alreadyCheckedBoard(checked)) pq.push(current->left);
			if (current->down && !current->down->alreadyCheckedBoard(checked)) pq.push(current->down);
			if (current->right && !current->right->alreadyCheckedBoard(checked)) pq.push(current->right);
			if (current->up && !current->up->alreadyCheckedBoard(checked)) pq.push(current->up);

			if (pq.size() > max_nodes_in_queue) { max_nodes_in_queue = pq.size(); }
		}
		return NULL;
	}

	// Traces the path back to the starting Node. Prints out the solution and number of steps
	void SolverTracer() {
		cout << "Total Steps: " << cost_g << endl << endl;

		Node* solution_node = this;
		while (solution_node->parent) {
			cout << "----------------------------------------------" << endl;
			cout << "Step g(n) + Heuristic h(n): " << solution_node->cost_g << " + " << solution_node->cost_h << " = " << (solution_node->cost_g + solution_node->cost_h) << endl;
			solution_node->printBoard();
			solution_node = solution_node->parent;
			
			cout << endl;
			
		}
		cout << "----------------------------------------------" << endl;
		cout << "Step g(n) + Heuristic h(n): " << solution_node->cost_g << " + " << solution_node->cost_h << " = " << (solution_node->cost_g + solution_node->cost_h) << endl;
		solution_node->printBoard();
		
	}
};

void print2dVector(vector<vector<int> > v) {
	for (unsigned i = 0; i < v.size(); i++) {
		for (unsigned j = 0; j < v.at(i).size(); j++) {
			cout << v.at(i).at(j) << " ";
		}
		cout << endl;
	}
	cout << endl;
}

int main() {
	vector<int> row1;
	vector<int> row2;
	vector<int> row3;
	vector<int> row4;
	vector<int> row5;

	while (true) {
		string input_rows, input_cols, input = "";
		vector<int> vrows;
		vector<vector<int> > start_board;
		vector < vector<int> > goal_board;

		// User setup of start_board
		cout << "Enter the STARTING puzzle state\n";
		cout << "1. Premade STARTING puzzle state\n"
			<< "2. Custom STARTING puzzle state\n";
		std::cin >> input;
		if (input == "2") {
			cout << "Enter number of rows: ";
			std::cin >> input_rows;
			cout << "Enter number of cols: ";
			std::cin >> input_cols;
			for (int i = 0; i < stoi(input_rows); i++) {
				cout << "Enter row " << i + 1 << " Use space between numbers. Use 0 for blank." << endl;
				for (int j = 0; j < stoi(input_cols); j++) {
					std::cin >> input;
					vrows.push_back(stoi(input));
				}
				start_board.push_back(vrows);
				vrows.clear();
			}
		}
		else {
			input_rows = "3";
			cout << "Choose premade 3x3 STARTING puzzle by number of solve steps\n"
				<< "2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26\n";
			std::cin >> input;
			switch (stoi(input)) {
			case 2:
				row1 = { 1, 2, 3 };
				row2 = { 4, 0, 6 };
				row3 = { 7, 5, 8 };
				break;
			case 4:
				row1 = { 0,2,3 };
				row2 = { 1,5,6 };
				row3 = { 4,7,8 };
				break;
			case 6:
				row1 = { 2,3,0 };
				row2 = { 1,5,6 };
				row3 = { 4,7,8 };
				break;
			case 8:
				row1 = { 4, 1, 3 };
				row2 = { 7, 2, 6};
				row3 = { 5, 8, 0 };
				break;
			case 10:
				row1 = { 5,1,2 };
				row2 = { 4,6,3 };
				row3 = { 0,7,8 };
				break;
			case 12:
				row1 = { 5,3,6 };
				row2 = { 2,0,1 };
				row3 = { 4,7,8 };
				break;
			case 14:
				row1 = { 7,4,2 };
				row2 = { 1,0,5 };
				row3 = { 8,6,3 };
				break;
			case 16:
				row1 = { 8, 1, 2 };
				row2 = { 4, 0, 3 };
				row3 = { 6, 7, 5 };
				break;
			case 18:
				row1 = { 0,7,6 };
				row2 = { 3,1,8 };
				row3 = { 5,2,4 };
				break;
			case 20:
				row1 = { 5, 4 ,3 };
				row2 = { 1, 0 ,6 };
				row3 = { 8, 2, 7 };
				break;
			case 22:
				row1 = { 5,6,2 };
				row2 = { 4,0,7 };
				row3 = { 3,1,8 };
				break;
			case 24:
				row1 = { 7,5,3 };
				row2 = { 6,4,1 };
				row3 = { 0,8,2 };
				break;
			case 26:
				row1 = { 8,5,7 };
				row2 = { 3,2,4 };
				row3 = { 0,1,6 };
				break;
			}
			start_board = { row1,row2,row3 };
		}

		system("CLS");

		// User setup of goal_board
		cout << "Enter the GOAL puzzle state\n";
		cout << "1. Default GOAL puzzle state (up to 5x5 puzzles)\n" <<
			"2. Custom GOAL puzzle state\n";
		std::cin >> input;
		if (input == "2") {
			cout << "Enter number of rows\n";
			std::cin >> input_rows;
			cout << "Enter number of cols\n";
			std::cin >> input_cols;
			for (int i = 0; i < stoi(input_rows); i++) {
				cout << "Enter row " << i + 1 << " Use space between numbers. Use 0 for blank." << endl;
				for (int j = 0; j < stoi(input_cols); j++) {
					std::cin >> input;
					vrows.push_back(stoi(input));
				}
				goal_board.push_back(vrows);
				vrows.clear();
			}
		}
		// Default goal_board
		else {
			switch (stoi(input_rows)) {
			case 2:
				row1 = { 1, 2 };
				row2 = { 3, 0 };
				goal_board = { row1, row2 };
				break;
			case 3:
				row1 = { 1, 2, 3 };
				row2 = { 4, 5, 6 };
				row3 = { 7, 8, 0 };
				goal_board = { row1, row2, row3 };
				break;
			case 4:
				row1 = { 1,2,3,4 };
				row2 = { 5,6,7,8 };
				row3 = { 9,10,11,12 };
				row4 = { 13,14,15,0 };
				goal_board = { row1, row2, row3, row4 };
				break;
			case 5:
				row1 = { 1,2,3,4,5 };
				row2 = { 6,7,8,9,10 };
				row3 = { 11,12,13,14,15 };
				row4 = { 16,17,18,19,20 };
				row5 = { 21,22,23,24,0 };
				goal_board = { row1, row2, row3, row4, row5 };
				break;
			}
			system("CLS");

			//// User setup of heuristic
			//cout << "Enter your choice of algorithm" << endl
			//	<< "1. Uniform Cost Search " << endl
			//	<< "2. A* with the Misplaced Tile heuristic " << endl
			//	<< "3. A* with the Manhattan distance heuristic " << endl;
			//std::cin >> input;
			//string heuristic;
			//switch (stoi(input)) {
			//case 1:
			//	heuristic = "Uniform_Cost_Search";
			//	break;
			//case 2:
			//	heuristic = "Misplaced_Tiles";
			//	break;
			//case 3:
			//	heuristic = "Manhattan";
			//	break;
			//default:
			//	heuristic = "Manhattan";
			//	break;
			//}
			//system("CLS");

			cout << "Start Board: " << endl;
			print2dVector(start_board);
			cout << "Goal Board: " << endl;
			print2dVector(goal_board);
			Node* root = new Node(start_board, goal_board);
			//root->Solver(heuristic)->SolverTracer();

			cout << "\n\nManhattan Distance\n";
			root->Solver("Manhattan")->SolverTracer();
			cout << "\n\nMisplaced Tiles\n";
			root->Solver("Misplaced_Tiles")->SolverTracer();
			cout << "\n\nUniform Cost Search\n"; 
			root->Solver("Uniform_Cost_Search")->SolverTracer();

			std::cin >> input;
			system("CLS");
		}
	}
	std::cin.get();
}
