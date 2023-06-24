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
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

using namespace std;

//============================================================================
// CSVParser Logic
//============================================================================

// Class to handle reading errors
class Error : public std::runtime_error
{

public:
    Error(const std::string& msg) :
        std::runtime_error(std::string("CSVparser : ").append(msg))
    {
    }
};

// Class to handle Row traversal
class Row
{
public:
    Row(const std::vector<std::string>&);

public:
    unsigned int size(void) const;
    void push(const std::string&);

private:
    const std::vector<std::string> _header;
    std::vector<std::string> _values;

public:

    template<typename T>
    const T getValue(unsigned int pos) const
    {
        if (pos < _values.size())
        {
            T res;
            std::stringstream ss;
            ss << _values[pos];
            ss >> res;
            return res;
        }
        throw Error("can't return this value (doesn't exist)");
    }
    const std::string operator[](unsigned int) const;
    const std::string operator[](const std::string& valueName) const;
};

// Enum for parse differentiation
enum DataType {
    eFILE = 0,
    ePURE = 1
};

// Class to handle parsing the file
class Parser
{

public:
    Parser(const std::string&, const DataType& type = eFILE, char sep = ',');

public:
    Row& getRow(unsigned int row) const;
    unsigned int rowCount(void) const;
    unsigned int columnCount(void) const;
    std::vector<std::string> getHeader(void) const;

protected:
    void parseHeader(void);
    void parseContent(void);

private:
    std::string _file;
    const DataType _type;
    const char _sep;
    std::vector<std::string> _originalFile;
    std::vector<std::string> _header;
    std::vector<Row*> _content;

public:
    Row& operator[](unsigned int row) const;
};

// Constructor
Parser::Parser(const std::string& data, const DataType& type, char sep)
    : _type(type), _sep(sep)
{
    std::string line;
    if (type == eFILE)
    {
        _file = data;
        std::ifstream ifile(_file.c_str());
        if (ifile.is_open())
        {
            while (ifile.good())
            {
                getline(ifile, line);
                if (line != "")
                    _originalFile.push_back(line);
            }
            ifile.close();

            if (_originalFile.size() == 0)
                throw Error(std::string("No Data in ").append(_file));

            parseHeader();
            parseContent();
        }
        else
            throw Error(std::string("Failed to open ").append(_file));
    }
    else
    {
        std::istringstream stream(data);
        while (std::getline(stream, line))
            if (line != "")
                _originalFile.push_back(line);
        if (_originalFile.size() == 0)
            throw Error(std::string("No Data in pure content"));

        parseHeader();
        parseContent();
    }
}

// Parse the first line of the file
void Parser::parseHeader(void)
{
    std::stringstream ss(_originalFile[0]);
    std::string item;

    while (std::getline(ss, item, _sep))
        _header.push_back(item);
}

// Parse the contents of the file
void Parser::parseContent(void)
{
    std::vector<std::string>::iterator it;

    it = _originalFile.begin();
    //it++; // skip header

    for (; it != _originalFile.end(); it++)
    {
        bool quoted = false;
        int tokenStart = 0;
        unsigned int i = 0;

        Row* row = new Row(_header);

        for (; i != it->length(); i++)
        {
            if (it->at(i) == '"')
                quoted = ((quoted) ? (false) : (true));
            else if (it->at(i) == ',' && !quoted)
            {
                row->push(it->substr(tokenStart, i - tokenStart));
                tokenStart = i + 1;
            }
        }

        //end
        row->push(it->substr(tokenStart, it->length() - tokenStart));

        // if value(s) missing
        //if (row->size() != _header.size())
            //throw Error("corrupted data !");
        _content.push_back(row);
    }
}

// Return the memory address of the current position
Row& Parser::getRow(unsigned int rowPosition) const
{
    if (rowPosition < _content.size())
        return *(_content[rowPosition]);
    throw Error("can't return this row (doesn't exist)");
}
Row& Parser::operator[](unsigned int rowPosition) const
{
    return Parser::getRow(rowPosition);
}

// Return the count of file records
unsigned int Parser::rowCount(void) const
{
    return _content.size();
}

// Return the count of file fields
unsigned int Parser::columnCount(void) const
{
    return _header.size();
}

// Return the first line of the file
std::vector<std::string> Parser::getHeader(void) const
{
    return _header;
}

// Constructor
Row::Row(const std::vector<std::string>& header)
    : _header(header) {}

// Return the size of the current row
unsigned int Row::size(void) const
{
    return _values.size();
}

// Add the current element to the list container
void Row::push(const std::string& value)
{
    _values.push_back(value);
}

// Return the current string element
const std::string Row::operator[](unsigned int valuePosition) const
{
    if (valuePosition < _values.size())
        return _values[valuePosition];
    throw Error("can't return this value (doesn't exist)");
}
const std::string Row::operator[](const std::string& key) const
{
    std::vector<std::string>::const_iterator it;
    int pos = 0;

    for (it = _header.begin(); it != _header.end(); it++)
    {
        if (key == *it)
            return _values[pos];
        pos++;
    }

    throw Error("can't return this value (doesn't exist)");
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
    Parser file = Parser(csvPath);

    try {
        // Read through the rows
        for (unsigned i = 0; i < file.rowCount(); i++) {
            
            // initialize a Course using data from current row (i)
            Course course;
            course.courseNum = file[i][0];
            course.courseTitle = file[i][1];

            // For each prereq within the current row
            for (unsigned j = 2; j < file.columnCount(); j++) {

                // Store current prerequisite
                string prerequisite = file[i][j];

                // Remove leading spaces
                prerequisite.erase(0, prerequisite.find_first_not_of(' ')); 

                // Loop through other rows to find a match
                for (unsigned k = 0; k < file.rowCount(); k++) {

                    // If the course ID is found
                    if (i != k && file[k][0] == prerequisite) {

                        // Insert match into current course prereqs
                        course.Prereqs.push_back(prerequisite);
                        break;
                    }
                }
            }
            // Insert populated course into the tree
            tree->Insert(course);
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