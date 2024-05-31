#include "network.h"
#include "user.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cctype>
#include <set>
#include <limits>
#include <queue>
#include <stack>
#include <algorithm>
Network::Network() {
    users_.clear();
}

Network::~Network() {
    for (User* user : users_) {
        delete user;
    }
    users_.clear();
}

void Network::addUser(User* user) {
    assert(user != nullptr); // Ensure user pointer is not null
    users_.push_back(user);
}

int Network::addConnection(const std::string& s1, const std::string& s2) {
    if (s1 == s2) {
        std::cerr << "Cannot add a friend connection to oneself." << std::endl;
        return -1;
    }
    
    int id1 = getId(s1);
    int id2 = getId(s2);

    if (id1 == -1 || id2 == -1) {
        if (id1 == -1) std::cerr << "User " << s1 << " not found." << std::endl;
        if (id2 == -1) std::cerr << "User " << s2 << " not found." << std::endl;
        return -1;
    }

    User* user1 = getUser(id1);
    User* user2 = getUser(id2);

    if (user1->getFriends().find(id2) != user1->getFriends().end()) {
        std::cerr << "Error: These users are already friends." << std::endl;
        return 0; // Duplicate connection
    }

    user1->addFriend(id2);
    user2->addFriend(id1);
    return 1; // Successful friendship
}



int Network::deleteConnection(const std::string& s1, const std::string& s2) {
    if (s1 == s2) return -1;
    int id1 = getId(s1);
    int id2 = getId(s2);
    if (id1 == -1 || id2 == -1) return -1;

    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1->getFriends().find(id2) == user1->getFriends().end()) {
        return -1; // No existing friendship to delete
    }

    user1->deleteFriend(id2);
    user2->deleteFriend(id1);
    return 0;
}

int Network::getId(const std::string& name) const {
    for (int i = 0; i < users_.size(); ++i) {
        if (users_[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}

User* Network::getUser(int id) const {
    if (id >= 0 && id < users_.size()) {
        return users_[id];
    }
    return nullptr; // or handle the error as needed
}


int Network::numUsers() const {
    return users_.size();
}

// Helper function to trim leading and trailing whitespace
std::string trim(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return ""; // All whitespace string
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

int Network::readUsers(const char* fname) {
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fname << std::endl;
        return -1;
    }

    int numUsers;
    file >> numUsers; // Read the number of users
    if (file.fail()) {
        std::cerr << "Failed to read the number of users." << std::endl;
        return -1;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip to the next line

    for (int i = 0; i < numUsers; ++i) {
        std::string line;
        int id, year, zip;
        std::string name, friendsLine;

        // Read ID
        std::getline(file, line);
        id = std::stoi(trim(line));

        // Read name
        std::getline(file, name);
        name = trim(name);

        // Read year
        std::getline(file, line);
        year = std::stoi(trim(line));

        // Read zip
        std::getline(file, line);
        zip = std::stoi(trim(line));

        // Read friends line
        std::getline(file, friendsLine);
        std::istringstream friendsStream(trim(friendsLine));
        std::set<int> friends;
        int friendId;
        while (friendsStream >> friendId) {
            friends.insert(friendId);
        }

        // Debug output
        //std::cout << "Read User - ID: " << id << ", Name: " << name
                  //<< ", Year: " << year << ", Zip: " << zip
                  //<< ", Friends Count: " << friends.size() << std::endl;

        // Construct and add the new user to the network
        User* newUser = new User(id, name, year, zip, friends);
        addUser(newUser);
    }

    file.close();
    return 0;
}


int Network::writeUsers(const char* fname) {
    std::ofstream file(fname);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << fname << ". Check if the path is correct and you have write permissions." << std::endl;
        return -1;
    }

    file << users_.size() << "\n";
    for (User* user : users_) {
        file << user->getId() << "\n" << user->getName() << "\n"
             << user->getYear() << "\n" << user->getZip() << "\n";
        for (int friendId : user->getFriends()) {
            file << friendId;
            if (friendId != *user->getFriends().rbegin()) file << " ";
        }
        file << "\n";
    }

    file.close();
    if (file.fail()) {
        std::cerr << "Failed to write data properly to " << fname << std::endl;
        return -1;
    }

    std::cout << "Data successfully written to " << fname << std::endl;
    return 0;
}

std::vector<int> Network::shortestPath(int from, int to) {
    if (from == to) {
        return {from}; // Shortest path to itself is the node itself
    }

    std::vector<bool> visited(numUsers(), false);
    std::vector<int> predecessor(numUsers(), -1);
    std::queue<int> queue;

    visited[from] = true;
    queue.push(from);

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        User* currentUser = getUser(current);
        for (int friendId : currentUser->getFriends()) {
            if (!visited[friendId]) {
                visited[friendId] = true;
                predecessor[friendId] = current;
                queue.push(friendId);
                if (friendId == to) {
                    std::vector<int> path;
                    for (int step = to; step != -1; step = predecessor[step]) {
                        path.push_back(step);
                    }
                    std::reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }

    return {}; // Return an empty path if no path exists
}
// Method to find a user at exactly 'distance' edges away from 'from'


std::vector<int> Network::distanceUser(int from, int& to, int distance) {
    if (from < 0 || from >= users_.size() || distance < 0) {
        std::cerr << "Invalid input parameters." << std::endl;
        to = -1;
        return {}; // Immediate return on invalid input.
    }

    std::vector<bool> visited(numUsers(), false);
    std::vector<int> dist(numUsers(), -1); // Distance initialization to -1
    std::vector<int> pred(numUsers(), -1); // Predecessor array
    std::queue<int> queue;
    std::vector<int> path;

    visited[from] = true;
    dist[from] = 0;
    queue.push(from);

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        std::cout << "Visiting node " << current << " at distance " << dist[current] << std::endl;

        // Check all neighbors of the current user
        for (int neighbor : getUser(current)->getFriends()) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                pred[neighbor] = current;
                dist[neighbor] = dist[current] + 1;
                std::cout << "Found neighbor " << neighbor << " at distance " << dist[neighbor] << std::endl;
                queue.push(neighbor);

                if (dist[neighbor] == distance) {
                    to = neighbor; // Set the found user's ID
                    std::cout << "Found target user " << to << " at the required distance " << distance << std::endl;

                    // Reconstruct the path using predecessors
                    for (int step = to; step != -1; step = pred[step]) {
                        path.push_back(step);
                    }
                    std::reverse(path.begin(), path.end());
                    std::cout << "Path: ";
                    for (int id : path) {
                        std::cout << getUser(id)->getName() << " -> ";
                    }
                    std::cout << "end." << std::endl;

                    return path; // Return the path as soon as the user is found
                }
            }
        }
    }

    to = -1; // No user found at the exact distance
    std::cout << "No user found at distance " << distance << std::endl;
    return {}; // Return empty path if no user is found
}

std::vector<int> Network::suggestFriends(int who, int& score) {
    User* user = getUser(who);
    if (!user) {
        score = 0;
        return std::vector<int>();
    }

    const std::set<int>& friends = user->getFriends();
    std::vector<int> commonFriendsCount(numUsers(), 0);
    std::vector<int> suggestions;
    score = 0;

    // Calculate scores for friend suggestions
    for (int friendId : friends) {
        User* friendUser = getUser(friendId);
        if (friendUser) {
            for (int friendsFriendId : friendUser->getFriends()) {
                // Skip if they're already friends or if it's the user themselves
                if (friends.find(friendsFriendId) != friends.end() || friendsFriendId == who) {
                    continue;
                }

                commonFriendsCount[friendsFriendId]++;
                score = std::max(score, commonFriendsCount[friendsFriendId]);
            }
        }
    }

    // Find all users with the highest score
    for (int i = 0; i < commonFriendsCount.size(); ++i) {
        if (commonFriendsCount[i] == score) {
            suggestions.push_back(i);
        }
    }

    // If no suggestions found, reset score to 0
    if (suggestions.empty()) {
        score = 0;
    }

    return suggestions;
}

void Network::dfs(int v, std::vector<bool>& visited, std::vector<int>& component) {
    std::stack<int> stack;
    stack.push(v);
    visited[v] = true;

    while (!stack.empty()) {
        int current = stack.top();
        stack.pop();
        component.push_back(current);

        for (int neighbor : getUser(current)->getFriends()) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                stack.push(neighbor);
            }
        }
    }
}

std::vector<std::vector<int>> Network::groups() {
    std::vector<std::vector<int>> components;
    std::vector<bool> visited(numUsers(), false);

    // Explore each user to find disconnected components
    for (int i = 0; i < numUsers(); i++) {
        if (!visited[i]) {
            std::vector<int> component;
            dfs(i, visited, component);
            components.push_back(component);
        }
    }

    return components;
}

