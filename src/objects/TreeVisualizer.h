#ifndef TREE_VISUALIZER_H
#define TREE_VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <map> // Required for potential node tracking (though simplified here)
#include <array> // Required for sf::Vertex array

#include "Algo.h" // Assuming Algo.h defines Algo::MoveStep

// Constants for visualization
const int WINDOW_WIDTH = 1200; // Increased width for potentially wider trees
const int WINDOW_HEIGHT = 1200;
const int NODE_RADIUS = 8;
// Define player colors - assuming player numbers are 0 and 1
const sf::Color PLAYER_COLORS[] = {sf::Color::Green, sf::Color::Red}; // Player 0: Blue, Player 1: Red

// Struct to represent a node in the visualization tree
struct VisualTreeNode {
    sf::Vector2f position;       // Screen position of the node
    algo::MoveStep move;         // The move that led *to* this node from its parent (dummy for root)
    VisualTreeNode* parent = nullptr; // Pointer to the parent node
    std::vector<VisualTreeNode*> children; // List of child nodes
    int depth = 0;               // Depth of the node in the tree (root is 0)
    int playerNumber = -1;       // Player who made the move to reach this node (-1 for root)
    int orderAtDepth = 0;        // Order of this node among its siblings at the same depth

    // Constructor
    VisualTreeNode(const sf::Vector2f& pos, const algo::MoveStep& m, VisualTreeNode* p, int d, int playerNum, int order)
        : position(pos), move(m), parent(p), depth(d), playerNumber(playerNum), orderAtDepth(order) {}

    // Destructor to clean up dynamically allocated children (recursive deletion)
    ~VisualTreeNode() {
        for (VisualTreeNode* child : children) {
            delete child;
        }
    }
};

class TreeVisualizer {
private:
    sf::RenderWindow window;              // The SFML window for drawing
    std::queue<algo::MoveStep> visualQueue; // Queue of move steps to visualize
    sf::Font font;                        // Font for displaying text

    VisualTreeNode* root = nullptr;       // The root node of the visualization tree
    VisualTreeNode* currentNode = nullptr; // Pointer to the node currently being visited/highlighted

    // Data structure to help with dynamic layout by tracking nodes at each depth
    // This helps in calculating horizontal positions as nodes are added.
    std::vector<std::vector<VisualTreeNode*>> nodesAtDepth;

    // Layout parameters - these will be adjusted
    float levelHeight = 20.0f;     // Vertical spacing between levels
    float horizontalSpacing = 50.0f; // Base horizontal spacing between nodes at the same level
    float startX = WINDOW_WIDTH / 2.0f; // X position for the root node (center of the window)
    float startY = 20.0f;        // Y position for the root node (near the top)

    // Keep track of the maximum width encountered at any depth during tree building
    int maxNodesAtAnyDepth = 1; // Starts with the root

public:
    // Constructor: Initializes the window and stores the visual queue
    TreeVisualizer(std::queue<algo::MoveStep>& visualQueue)
        : window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Backtracking Tree Visualization"),
          visualQueue(visualQueue) {

        // Set the frame rate limit for smooth animation
        window.setFramerateLimit(60);

        // Load the font. Replace "arial.ttf" with the actual path to your font file.
        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "Failed to load font. Make sure arial.ttf is in the correct path." << std::endl;
            // In a real application, you might want to handle this error more robustly,
            // e.g., use a default font or exit if font loading is critical.
        }

        // Create the root node of the visualization tree.
        // The root represents the initial state before any moves are made.
        // It doesn't correspond to a move itself.
        // Initialize the dummy move for the root, including the isForwards flag.
        algo::MoveStep rootMove = {{ -1, -1 }, { -1, -1 }, -1, false}; // Dummy move for the root node, isForwards=false as it's not a traversal step
        root = new VisualTreeNode({startX, startY}, rootMove, nullptr, 0, -1, 0); // Depth 0, Player -1 for root, Order 0
        currentNode = root; // The visualization starts at the root node

        // Add the root node to the tracking structure at depth 0
        nodesAtDepth.push_back({root});
    }

    // Destructor: Cleans up the dynamically allocated tree nodes
    ~TreeVisualizer() {
        delete root; // The destructor of VisualTreeNode is recursive and will delete all children
    }

    // Runs the visualization loop
    void run() {
        sf::Clock clock; // Clock to manage animation speed
        float animationSpeed = 0.1f; // Time delay in seconds between processing each step
        float timeAccumulator = 0; // Accumulates elapsed time

        // Main window loop
        while (window.isOpen()) {
            // Process events (like closing the window)
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
                // Add other event handling here (e.g., keyboard input for pausing/stepping)
            }

            // Update visualization based on time and queue state
            timeAccumulator += clock.restart().asSeconds(); // Add time since last frame

            // If enough time has passed and there are steps left in the queue, process the next step
            if (timeAccumulator >= animationSpeed && !visualQueue.empty()) {
                timeAccumulator = 0; // Reset the accumulator
                processNextStep(); // Process one move/revert step from the queue
                // adjustLayout(); // Call the layout adjustment function
            } else if (visualQueue.empty()) {
				adjustLayout();
				window.clear(sf::Color::White); // Clear the window with a background color (white)

				drawTree(root); // Draw the current state of the dynamically built tree starting from the root
				drawCurrentNodeHighlight(); // Draw a highlight around the node currently being visited

				// Draw information about the step being processed (or the next one in the queue)
				drawMoveInfo();

				window.display(); // Display the drawn elements on the window
                // Keep displaying the final state of the tree for a few seconds before closing.
                 sf::sleep(sf::seconds(3.0)); // Display the final tree state for 3 seconds
                 window.close(); // Close the window after the delay
                 // Alternatively, you could add logic here to wait for user input to close the window.
            }

            // --- Drawing ---
            window.clear(sf::Color::White); // Clear the window with a background color (white)

            drawTree(root); // Draw the current state of the dynamically built tree starting from the root
            drawCurrentNodeHighlight(); // Draw a highlight around the node currently being visited

            // Draw information about the step being processed (or the next one in the queue)
            drawMoveInfo();

            window.display(); // Display the drawn elements on the window
        }
    }

private: // Helper methods for visualization logic and drawing

    // Processes one MoveStep from the visualQueue and updates the tree visualization
    void processNextStep() {
        // Get the next move step from the front of the queue
        algo::MoveStep nextStep = visualQueue.front();
        visualQueue.pop(); // Remove the step from the queue

        // Print the step being processed for debugging/console view
        std::cout << "Processing Step: Player " << nextStep.playerNumber
                  << " (" << nextStep.from.first << "," << nextStep.from.second << ") -> (" // Use 'from' and 'to' as per struct
                  << nextStep.to.first << "," << nextStep.to.second << ")"
                  << (nextStep.isForwards ? " (Forward)" : " (Backward)") << std::endl; // Indicate direction


        // Use the isForwards flag to determine the type of step
        if (!nextStep.isForwards) {
            // If it's a backward move, move the 'currentNode' pointer up to its parent.
            // Ensure we are not already at the root before moving up.
            if (currentNode->parent != nullptr) {
                currentNode = currentNode->parent;
                std::cout << "  -> Moved back to parent node." << std::endl;
            } else {
                 // This case should ideally not happen if the queue is correctly formed,
                 // as you shouldn't have a backward step from the root's conceptual state.
                 std::cerr << "Warning: Received a backward step while at the root node." << std::endl;
            }
        } else { // isForwards is true
            // If it's a forward move (isForwards is true).
            // This means we are moving from the state represented by the currentNode
            // to a new state represented by the nextStep.

            // Look for an existing child of the currentNode that was created by this exact move step.
            VisualTreeNode* existingChild = nullptr;
            for (VisualTreeNode* child : currentNode->children) {
                // Compare the move details to find a matching child
                if (child->move.from.first == nextStep.from.first && // Use 'from' and 'to' for comparison
                    child->move.from.second == nextStep.from.second &&
                    child->move.to.first == nextStep.to.first &&
                    child->move.to.second == nextStep.to.second &&
                    child->move.playerNumber == nextStep.playerNumber &&
                    child->move.isForwards == nextStep.isForwards) { // Also compare isForwards flag
                    existingChild = child; // Found an existing node for this move
                    break;
                }
            }

            if (existingChild) {
                // If an existing child node is found for this move, move the 'currentNode' pointer to it.
                // This happens when the search backtracks up a branch and then goes down another branch
                // that was previously visited.
                currentNode = existingChild;
                 std::cout << "  -> Forward move to existing child node." << std::endl;
            } else {
                // If no existing child is found for this move, it means we are exploring a new state
                // for the first time along this path. Create a new child node.
                 std::cout << "  -> Forward move creating a new child node." << std::endl;

                int newDepth = currentNode->depth + 1; // The new node is one level deeper than the current node

                // --- Calculate Position for the New Node ---
                sf::Vector2f newNodePos;
                // This is a simplified dynamic layout strategy. It positions nodes based on their depth
                // and their order of creation at that depth. This can lead to overlapping nodes
                // for complex or wide trees. A proper tree layout algorithm is more sophisticated
                // but harder to implement for dynamic building.

                // Ensure nodesAtDepth vector has enough levels to store nodes at the new depth
                if (nodesAtDepth.size() <= newDepth) {
                    nodesAtDepth.resize(newDepth + 1);
                }

                // Get the number of nodes already added to this new depth level.
                // This will be the index/order of the new node within this level.
                int orderAtDepth = nodesAtDepth[newDepth].size();

                // Calculate horizontal position: Spread nodes out based on their order at the depth.
                // The total width needed for the level is estimated, and nodes are centered.
                float levelCenter = WINDOW_WIDTH / 2.0f; // Center of the window
                float nodesInCurrentLevel = nodesAtDepth[newDepth].size() + 1; // Number of nodes including the one being added
                float totalLevelWidth = nodesInCurrentLevel * horizontalSpacing; // Estimated total width for this level

                // Calculate the starting X position for the level to center it
                float levelStartX = levelCenter - totalLevelWidth / 2.0f;

                // Calculate the X position for the new node within this level
                newNodePos.x = levelStartX + (orderAtDepth + 0.5f) * horizontalSpacing; // Position node at the center of its allocated horizontal space

                // Calculate the Y position for the new node based on its depth
                newNodePos.y = startY + newDepth * levelHeight;


                // Create the new VisualTreeNode object
                VisualTreeNode* newNode = new VisualTreeNode(newNodePos, nextStep, currentNode, newDepth, nextStep.playerNumber, orderAtDepth); // Pass orderAtDepth

                // Add the new node to the children list of the current node (its parent)
                currentNode->children.push_back(newNode);

                // Update 'currentNode' to point to the newly created child node
                currentNode = newNode;

                // Add the new node to the tracking structure for layout help
                nodesAtDepth[newDepth].push_back(newNode);

                // Update max width encountered for potential future layout adjustments
                if (nodesAtDepth[newDepth].size() > maxNodesAtAnyDepth) {
                    maxNodesAtAnyDepth = nodesAtDepth[newDepth].size();
                    // You could potentially adjust horizontalSpacing here based on the widest level,
                    // but re-positioning existing nodes dynamically is complex.
                }
            }
        }
    }

    // Recursively draws the tree structure starting from the given node
    void drawTree(VisualTreeNode* node) {
        if (!node) return; // Base case for recursion

        // Draw edges from this node to all its children *before* drawing the node itself.
        // This ensures edges are drawn underneath the nodes.
        for (VisualTreeNode* child : node->children) {
            drawEdge(node->position, child->position);
        }

        // Draw the current node
        drawNode(node);

        // Recursively call drawTree for each child node to draw their subtrees
        for (VisualTreeNode* child : node->children) {
            drawTree(child);
        }
    }

    // Draws a single node (a circle with optional text)
    void drawNode(VisualTreeNode* node) {
        sf::CircleShape shape(NODE_RADIUS);
        // Position the circle shape. Its origin is top-left by default, so subtract radius to center it on the node's position.
        shape.setPosition(node->position - sf::Vector2f(NODE_RADIUS, NODE_RADIUS));

        // Set the fill color of the node based on the player number (or depth, etc.)
        // Assuming playerNumber is 0 or 1. Root node has playerNumber -1.
        int colorIndex = (node->playerNumber == -1) ? 0 : node->playerNumber % 2; // Use player 0 color for root or if playerNumber is not 0 or 1
        shape.setFillColor(PLAYER_COLORS[colorIndex]);

        // Optional: Draw node identifier text (e.g., depth or a simplified state ID)
         sf::Text idText(font, "", 12);
         idText.setFont(font); // Use the loaded font
         // You might want a more meaningful identifier here if possible
         idText.setString(std::to_string(node->depth)); // Displaying depth for simplicity
         idText.setCharacterSize(12); // Font size for node text
         idText.setFillColor(sf::Color::Black); // Text color

         // Center the text on the node's position
         sf::FloatRect textBounds = idText.getLocalBounds();
         idText.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
         idText.setPosition(node->position);

        window.draw(shape); // Draw the node circle
        window.draw(idText); // Draw the node identifier text
    }

    // Draws an edge (a line segment) between two points
    void drawEdge(const sf::Vector2f& from, const sf::Vector2f& to) {
        // Use std::array for sf::Vertex as recommended for SFML 3.0
        std::array<sf::Vertex, 2> line = {
            sf::Vertex{from, sf::Color(150, 150, 150)}, // Start point with color (grey)
            sf::Vertex{to, sf::Color(150, 150, 150)}   // End point with color (grey)
        };
        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines); // Draw the line primitive
    }

    // Draws a highlight circle around the currently active node
    void drawCurrentNodeHighlight() {
        if (currentNode) {
            sf::CircleShape highlight(NODE_RADIUS + 3); // Create a slightly larger circle for the highlight
            // Position the highlight circle, centered on the currentNode's position
            highlight.setPosition(currentNode->position - sf::Vector2f(NODE_RADIUS + 3, NODE_RADIUS + 3));
            highlight.setFillColor(sf::Color::Yellow); // Set the highlight color to yellow
            window.draw(highlight); // Draw the highlight circle
        }
    }

    // Draws information about the move step currently being processed (or the next one in the queue)
    void drawMoveInfo() {
        sf::Text infoText(font, "", 24);
        infoText.setFont(font); // Use the loaded font

        if (!visualQueue.empty()) {
             // Display information about the step that is *about to be* processed
             const algo::MoveStep& nextStep = visualQueue.front();
             infoText.setString(
                "Next Step:\n"
                "Player: " + std::to_string(nextStep.playerNumber) + "\n" +
                "From: (" + std::to_string(nextStep.from.first) + "," + std::to_string(nextStep.from.second) + ")\n" +
                "To: (" + std::to_string(nextStep.to.first) + "," + std::to_string(nextStep.to.second) + ")\n" +
                "Direction: " + (nextStep.isForwards ? "Forward" : "Backward")
             );
             // Set text color based on the player of the next step
             infoText.setFillColor(PLAYER_COLORS[nextStep.playerNumber == -1 ? 0 : nextStep.playerNumber % 2]);
        } else {
            // Display a message when the visualization is complete
            infoText.setString("Visualization Complete.");
            infoText.setFillColor(sf::Color::Black); // Black color for completion message
        }

        infoText.setCharacterSize(18); // Font size for the info text
        // Position the text at the bottom left of the window with some padding
        infoText.setPosition(sf::Vector2f{20.0f, WINDOW_HEIGHT - 150.0f});
        window.draw(infoText); // Draw the information text
    }

    // Function to adjust layout based on the maximum depth and width of the tree
    void adjustLayout() {
        // Calculate the maximum depth reached in the tree.
        // nodesAtDepth.size() gives the number of levels (depth 0 is the first level).
        // Max depth is the number of levels minus 1.
        int maxDepth = nodesAtDepth.empty() ? 0 : nodesAtDepth.size() - 1;

        // The maximum number of nodes at any single depth is tracked in maxNodesAtAnyDepth.
        // We can also verify this by iterating through nodesAtDepth, but the member variable should be correct.
        int widestLevelNodeCount = maxNodesAtAnyDepth;

        // Define padding for the window edges
        float verticalPadding = 100.0f; // Padding at the top and bottom
        float horizontalPadding = 50.0f; // Padding on the left and right

        // Calculate the new level height to fit the tree vertically within the window
        if (maxDepth > 0) {
            // Available vertical space is WINDOW_HEIGHT minus padding. Divide by maxDepth to get spacing between levels.
            levelHeight = (WINDOW_HEIGHT - verticalPadding) / static_cast<float>(maxDepth);
            // Ensure a minimum level height to prevent nodes from being too close
            if (levelHeight < 50.0f) levelHeight = 40.0f; // Minimum vertical spacing
        } else {
            // If maxDepth is 0 (only root node), use a default level height
            levelHeight = 100.0f;
        }

        // Calculate the new horizontal spacing to fit the widest level horizontally
        if (widestLevelNodeCount > 1) {
            // Available horizontal space is WINDOW_WIDTH minus padding.
            // Divide by (widestLevelNodeCount - 1) to get spacing between the centers of nodes at the widest level.
            horizontalSpacing = (WINDOW_WIDTH - horizontalPadding) / static_cast<float>(widestLevelNodeCount - 1);
            // Ensure a minimum horizontal spacing
            if (horizontalSpacing < 40.0f) horizontalSpacing = 40.0f; // Minimum horizontal spacing
        } else {
            // If the widest level has only one node, use a default large spacing (or the full width)
            horizontalSpacing = WINDOW_WIDTH - horizontalPadding; // Essentially centers the single node
        }

        // --- Recalculate Positions for All Nodes ---
        // Iterate through all nodes in the tree and update their screen positions
        // based on the newly calculated levelHeight and horizontalSpacing.
        for (int depth = 0; depth < nodesAtDepth.size(); ++depth) {
            // Get the number of nodes at the current depth
            float currentLevelNodeCount = nodesAtDepth[depth].size();
            if (currentLevelNodeCount == 0) continue; // Should not happen for existing levels

            // Calculate the total width needed for nodes at this level based on the new spacing
            // This is (number of nodes - 1) * spacing between centers
            float totalLevelWidth = (currentLevelNodeCount - 1) * horizontalSpacing;

            // Calculate the starting X position for this level to center it in the window
            float levelStartX = WINDOW_WIDTH / 2.0f - totalLevelWidth / 2.0f;

            // Iterate through each node at the current depth and update its position
            for (int i = 0; i < nodesAtDepth[depth].size(); ++i) {
                VisualTreeNode* node = nodesAtDepth[depth][i];

                // Calculate the node's X position based on its order at this depth and the horizontal spacing
                node->position.x = levelStartX + i * horizontalSpacing;

                // Calculate the node's Y position based on its depth and the new levelHeight
                node->position.y = startY + depth * levelHeight; // Use the new levelHeight
            }
        }

        // Ensure the root node's position is correctly updated based on the new startY and centered X
        if (root) {
             root->position = {WINDOW_WIDTH / 2.0f, startY};
        }
    }
};

#endif // TREE_VISUALIZER_H
