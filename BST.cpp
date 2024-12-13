#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <string>

using namespace std;

// Tree node definition
struct TreeNode {
    string value;
    sf::Vector2f position;
    int left;  // Index of the left child in the nodes vector (-1 if no left child)
    int right; // Index of the right child in the nodes vector (-1 if no right child)
};

// Node class for tree building
class Node {
public:
    Node* left;
    Node* right;
    int data;

    Node(int data) {
        this->data = data;
        this->left = NULL;
        this->right = NULL;
    }
};

// Helper function to find position in the inorder array
int findpos(int arr[], int n, int element) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == element) {
            return i;
        }
    }
    return -1;
}

// Build tree from inorder and preorder traversals
Node* buildtree(vector<int>& arr, int& index)
{
    if (index >= arr.size() || arr[index] == -1)
    {
        index++;
        return nullptr;
    }
    Node* root = new Node(arr[index++]);
    root->left = buildtree(arr, index);
    root->right = buildtree(arr, index);
    return root;
}

// Function to draw an arrow from start to end
void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end) {
    sf::VertexArray arrow(sf::LinesStrip, 2);
    arrow[0].position = start;
    arrow[1].position = end;
    arrow[0].color = sf::Color::White;
    arrow[1].color = sf::Color::White;

    // Calculate the direction vector and its length
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length; // Normalize the direction

    // Shorten the arrow by some fraction of the original length (e.g., 20%)
    float shortenFactor = 0.8f; // Arrow will be 80% of the original length
    sf::Vector2f newEnd = start + direction * (length * shortenFactor);

    // Draw the shortened arrow
    arrow[1].position = newEnd;
    window.draw(arrow);

    // Adjust the arrowhead position
    sf::VertexArray arrowhead(sf::Triangles, 3);
    sf::Vector2f perp(-direction.y, direction.x);
    perp *= 10.0f; // Arrowhead size

    sf::Vector2f middle = newEnd - direction * 10.0f; // Arrowhead base

    arrowhead[0].position = newEnd;
    arrowhead[1].position = middle + perp;
    arrowhead[2].position = middle - perp;

    arrowhead[0].color = sf::Color::White;
    arrowhead[1].color = sf::Color::White;
    arrowhead[2].color = sf::Color::White;

    window.draw(arrowhead);
}

// Function to set the positions of the nodes automatically
void setPositions(vector<TreeNode>& nodes, int windowWidth, int windowHeight) {
    if (nodes.empty()) return;

    queue<int> q;
    q.push(0);

    int level = 0;
    int nodeCountAtLevel = 1;  // Initialize to 1 for the root node

    vector<int> levelWidths;  // To hold the maximum number of nodes at each level

    // First pass: determine the number of nodes at each level
    while (!q.empty()) {
        int levelNodes = q.size();
        levelWidths.push_back(levelNodes);
        for (int i = 0; i < levelNodes; ++i) {
            int nodeIndex = q.front();
            q.pop();
            if (nodes[nodeIndex].left != -1) q.push(nodes[nodeIndex].left);
            if (nodes[nodeIndex].right != -1) q.push(nodes[nodeIndex].right);
        }
    }

    // Second pass: set positions based on level widths
    q.push(0);
    level = 0;
    while (!q.empty()) {
        int levelNodes = q.size();
        float xOffset = windowWidth / (levelWidths[level] + 1);  // Adjust based on actual number of nodes at this level
        float yOffset = 100.0f;  // Vertical distance between levels

        for (int i = 0; i < levelNodes; ++i) {
            int nodeIndex = q.front();
            q.pop();
            nodes[nodeIndex].position = sf::Vector2f(xOffset * (i + 1), yOffset * (level + 1));
            if (nodes[nodeIndex].left != -1) q.push(nodes[nodeIndex].left);
            if (nodes[nodeIndex].right != -1) q.push(nodes[nodeIndex].right);
        }
        level++;
    }
}


// Function to perform level-order traversal and convert to vector<TreeNode>
void levelOrderTraversal(Node* root, vector<TreeNode>& nodes) {
    if (!root) return;

    queue<Node*> q;
    q.push(root);
    int index = 0;

    while (!q.empty()) {
        Node* temp = q.front();
        q.pop();

        TreeNode treeNode;
        treeNode.value = to_string(temp->data);
        treeNode.left = treeNode.right = -1;

        if (temp->left) {
            treeNode.left = ++index;
            q.push(temp->left);
        }
        if (temp->right) {
            treeNode.right = ++index;
            q.push(temp->right);
        }

        nodes.push_back(treeNode);
    }
}

int main() {
    // Create the main window
    /*int inorder[] = { 40, 20, 50, 10, 60, 30, 70 };
    int preorder[] = { 10, 20, 40, 50, 30, 60, 70 };
    int size = 7;
    int preindex = 0;
    int inorderstart = 0;
    int inorderend = size - 1;*/

    // Build the tree
    vector<int> arr = { 50,30,20,10,5,-1,-1,15,-1,-1,35,25,-1,-1 };
    int index = 0;
    Node* root = buildtree(arr, index);

    // Perform level-order traversal and convert to vector<TreeNode>
    vector<TreeNode> nodes;
    levelOrderTraversal(root, nodes);

    // Automatically set the positions of the nodes
    setPositions(nodes, 1040, 800);

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(1040, 800), "Binary Tree Visualization");

    // Load a font
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Create circles and texts based on node positions
    vector<sf::CircleShape> circles;
    vector<sf::Text> texts;

    for (const auto& node : nodes) {
        sf::CircleShape circle(50); // Radius of the circle
        circle.setFillColor(sf::Color::Cyan);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(2);
        circle.setPosition(node.position.x - circle.getRadius(), node.position.y - circle.getRadius());
        circles.push_back(circle);

        sf::Text text;
        text.setFont(font);
        text.setString(node.value);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(node.position.x, node.position.y - 10);
        texts.push_back(text);
    }

    // Main game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear screen
        window.clear();

        // Draw the circles and texts
        for (size_t i = 0; i < circles.size(); ++i) {
            window.draw(circles[i]);
            window.draw(texts[i]);
        }

        // Draw arrows connecting the circles
        for (const auto& node : nodes) {
            if (node.left != -1) {
                drawArrow(window, node.position, nodes[node.left].position);
            }
            if (node.right != -1) {
                drawArrow(window, node.position, nodes[node.right].position);
            }
        }

        // Update the window
        window.display();
    }

    return 0;
}