//============================================================================
// Name        : Course_Offerings.cpp
// Author      : Nolan Pratt
// Version     : 1.0
// Description : Solution for reading in course information from a CSV file
//               and storing elements within a binary search tree for use
//               within an academic portal environment
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// CSV Parser Logic
//============================================================================

// Class for handle file reading errors
class Error : public std::runtime_error
{

public:
    Error(const std::string& msg) :
        std::runtime_error(std::string("CSVparser : ").append(msg))
    {
    }
};

// Class for handling file parsing
class Parse {
public:
    Parse();
    vector<vector<string>> parseContent(string filename);
};

// Constructor
Parse::Parse() {
    
}

/*
* Parse the file contents
* 
* @return 2d vector containing indexed file lines
* @param filename, filename of the CSV file
*/
vector<vector<string>> Parse::parseContent(string filename) {
    // 2d vector for storing line elements
    vector<vector<string>> content;

    // String element to track line elements
    string line;

    // Stream element for reading in file elements
    ifstream file(filename.c_str());

    // If the file successfully opened
    if (file.is_open()) {
        if (file.good()) {
            // While the file is open
            while (getline(file, line)) {
                // Store each line
                vector<string> row;

                // Read in each line
                stringstream line_stream(line);

                // Break each line into cells
                string cell;

                // Until the current line ends
                while (getline(line_stream, cell, ',')) {
                    // Insert the cells into the row container
                    row.push_back(cell);
                }

                // Insert the row into the rows containers
                content.push_back(row);
            }
        }
    }
    return content;
}

//============================================================================
// Binary Search Tree Logic
//============================================================================

// Structure to hold course information
struct Course {
    string courseNum;
    string courseTitle;
    vector<string> Prereqs;
};

// Structure for each node within the tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // Initialize with a Course
    Node(Course newCourse) : Node() {
        course = newCourse;
    }
};

// Class declaring bst structure
class BinarySearchTree {
private:
	Node* root;

    void inOrder(Node* node);
    void addNode(Node* node, Course course);

public:
    BinarySearchTree();
    void Insert(Course course);
	Course Search(string courseNum);
    void InOrder();
};

/*
* Constructor
*/
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/*
* Insert a course into the tree
*/
void BinarySearchTree::Insert(Course course) {
    // If the root is empty
    if (root == nullptr) {
        // Root filled by new node with bid parameter
        root = new Node(course);
    }
    // Otherwise, call addNode logic to find node placement
    else {
        addNode(root, course);
    }
}

/*
* Private call for the Insert method:
* Recursively search through the tree for an 
* open match to insert a populated node
* 
* @param node, Current node in tree
* @param course, Course to be added
*/
void BinarySearchTree::addNode(Node* node, Course course) {
    // If the node's courseId is greater than the parameter's
    if (node->course.courseNum > course.courseNum) {

        // If the left node is empty
        if (node->left == nullptr) {
            // Fill the left node
            node->left = new Node(course);
        }

        // Traverse to the left child node to find placement
        else {
            addNode(node->left, course);
        }
    }

    // If the node's courseId is less than the parameter's
    else {

        // if the right node is empty
        if (node->right == nullptr) {
            // Fill the right node
            node->right = new Node(course);
        }

        // Traverse to the right child node to find placement
        else {
            addNode(node->right, course);
        }
    }
}

/*
* Search for a node within the tree
* 
* @param courseId, courseId to search
* @return Course, course found or blank course
*/
Course BinarySearchTree::Search(string courseNum) {
    // Begin search from the root
    Node* currNode = root;

    // While the current node is not empty
    while (currNode != nullptr) {

        // If the current node's courseId matches the parameter
        if (currNode->course.courseNum == courseNum) {
            // Return the current Node's bid
            return currNode->course;
        }

        // If the current node's courseId is less than the left child
        else if (courseNum < currNode->course.courseNum) {
            // Traverse the left subtree
            currNode = currNode->left;
        }

        // If the current node's courseId is greater than the left child
        else {
            // Traverse the right subtree
            currNode = currNode->right;
        }
    }

    // Return default course if not found
    return Course();
}

/*
* Traverse the tree in alphanumeric order
*/
void BinarySearchTree::InOrder() {
    inOrder(root);
}

/*
* Private call for the InOrder method:
* Print all tree nodes in alphanumeric order
*/
void BinarySearchTree::inOrder(Node* node) {
    // If the current node is not empty
    if (node != nullptr) {

        // Traverse left subtree
        inOrder(node->left);

        // Print current node's Course info
        cout << node->course.courseNum << ": "
            << node->course.courseTitle << endl;

        // Print Course Prerequisites if applicable
        if (node->course.Prereqs.size() >= 1) {
            cout << "Prerequisites: ";
            for (int i = 0; i < node->course.Prereqs.size(); i++) {
                if (i == node->course.Prereqs.size() - 1) {
                    cout << node->course.Prereqs.at(i);
                    break;
                }
                cout << node->course.Prereqs.at(i) << ", ";
            }
        }
        else {
            cout << "No Prerequisites.";
        }
        cout << endl << endl;

        // Traverse right subtree
        inOrder(node->right);
    }
}

/**
 * Load a CSV file containing courses into a BST
 *
 * @return a BST containing all courses
 * @param csvPath, Path to the csv file
 * @param tree, Pointer to the working tree
 */
void loadCourses(string csvPath, BinarySearchTree* tree) {
    cout << "Loading CSV file " << csvPath << endl << endl;;

    // initialize the CSV Parser
    Parse file;
    vector<vector<string>> content = file.parseContent(csvPath);

    try {
        // Read through the rows
        for (unsigned i = 0; i < content.size(); i++) {
            // Empty course for placement
            Course course;

            if (content[i][0] != "") {

                // Check if the course is already in the tree to avoid data duplication
                if (tree->Search(content[i][0]).courseNum != "") {
                    continue;
                }

                // initialize a Course using data from current row (i)
                course.courseNum = content[i][0];
                course.courseTitle = content[i][1];

                // For each prereq within the current row
                for (unsigned j = 2; j < content[i].size(); j++) {

                    // Store current prerequisite
                    string prerequisite = content[i][j];

                    // Remove trailing and leading spaces
                    prerequisite.erase(0, prerequisite.find_first_not_of(' '));
                    prerequisite.erase(prerequisite.find_last_not_of(' ') + 1);

                    // Loop through other rows to find a match
                    for (unsigned k = 0; k < content.size(); k++) {

                        // If the course ID is found
                        if (content[k][0] == prerequisite && i != k && prerequisite != "") {

                            // Insert match into current course prereqs
                            course.Prereqs.push_back(prerequisite);
                            break;
                        }
                    }
                }
            }

            // If the courseNum is not the header nor blank
            if (!(course.courseNum == "courseId" || course.courseNum == "courseNum" || course.courseNum == "")) {
                // Insert populated course into the tree
                tree->Insert(course);
            }
        }
    }
    catch (Error& e) {
        std::cerr << e.what() << std::endl;
    }
}

/*
 * Display menu, read in user selections
 */
void Menu() {

    int selection = 0;
    string name;
    Course course;
    BinarySearchTree* tree;
    tree = new BinarySearchTree();

    // Inform user of case sensitivity upon initial access
    cout << "Reminder: input is case sensitive." << endl << endl;

    while (selection != 9) {
        // Print options
        cout << "Select an option:\n"
            << "1. Load Courses\n"
            << "2. Print Course List\n"
            << "3. Search Course Number\n"
            << "9. Exit\n" << endl;

        // Read in user input
        cin >> selection;

        // Handle user input
        switch (selection) {

        // Load courses
        case (1):
            cout << "Enter file name:" << endl;
            cin >> name;

            loadCourses(name, tree);
            break;

        // Print courses
        case (2):
            cout << endl;
            tree->InOrder();
            break;

        // Search courses
        case (3):
            // Gather course number
            cout << "Enter course number:" << endl;
            cin >> name;

            // Search for course
            course = tree->Search(name);

            // Print course info if found
            if (course.courseNum != "") {
                cout << endl << "Course found: " << endl
                    << course.courseNum << ", "
                    << course.courseTitle << endl
                    << "Prerequisites: ";

                // Print prerequisites
                for (int i = 0; i < course.Prereqs.size(); i++) {
                    if (i == course.Prereqs.size() - 1) {
                        cout << course.Prereqs.at(i);
                        break;
                    }
                    cout << course.Prereqs.at(i) << ", ";
                }
                cout << endl << endl;
            }

            // Otherwise, print not found
            else {
                cout << "\nCourse not found.\n" << endl;;
            }
            break;

        // Exit program
        case (9):
            cout << endl << "Thank you for using the course planner!" << endl;
            break;
        }
    }
}


/*
 * The main function of the program
 */
int main() {
    Menu();
	return 0;
}