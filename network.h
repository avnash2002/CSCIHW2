#ifndef NETWORK_H
#define NETWORK_H

#include "user.h"
#include <vector>
#include <string>

class Network {
private:
    std::vector<User*> users_; // Stores pointers to User objects

public:
    //Default Constructor
    // Pre-condition: None
    // Post-condition: Creates an empty network
    Network();
    
    //Destructor
    // Pre-condition: deletesall User objects in the network
    // Post-condition: All User objects are deleted
    ~Network();

    // Adds a User to the network
    // Pre-condition: user pointer must not be null
    // Post-condition: The user is added to the network
    void addUser(User* user);

    // Determines shortest path between two users 
    // Pre-condition: must not be a duplicate user or non existent user
    // Post-condition: Returns shortest path vector containing listed path
    std::vector<int> shortestPath(int startUserId, int endUserId);

    // Establishes a friendship between two users based on their names
    // Pre-condition: Both names must exist in the network
    // Post-condition: Returns 0 on success, -1 if either user does not exist or names are the same
    int addConnection(const std::string& s1, const std::string& s2);

    // Removes a friendship between two users based on their names
    // Pre-condition: Both names must exist in the network
    // Post-condition: Returns 0 on success, -1 if either user does not exist
    int deleteConnection(const std::string& s1, const std::string& s2);

    // Retrieves a user's ID by their name
    // Pre-condition: The name must be a valid string
    // Post-condition: Returns the user's ID or -1 if the user does not exist
    int getId(const std::string& name) const;

    // Retrieves a pointer to the User with the given ID
    // Pre-condition: ID must be within the range of existing user indices
    // Post-condition: Returns a pointer to the User or nullptr if the ID is invalid
    User* getUser(int id) const;

    // Returns the number of users in the network
    // Pre-condition: None
    // Post-condition: Returns the count of users
    int numUsers() const;

    // Reads users from a file
    // Pre-condition: fname must point to a valid filename
    // Post-condition: Returns 0 on success, -1 on failure
    int readUsers(const char* fname);

    // Writes users to a file
    // Pre-condition: fname must point to a valid filename
    // Post-condition: Returns 0 on success, -1 on failure
    int writeUsers(const char* fname);

    // Prints path of user from a specified user and distance
    // pre-condition: must be a valid user
    // Post-condition: returns vector path 
    std::vector<int> distanceUser(int from, int& to, int distance);
    
    // Finds groups of users within the network
    // Pre-condition: None
    // Post-condition: Returns a vector of vectors representing groups of users
    std::vector<std::vector<int>> groups();

    // Suggests friends for a given user
    // Pre-condition: who must be a valid user ID
    // Post-condition: Returns a vector of suggested friends and updates the score
    std::vector<int> suggestFriends(int who, int& score);

    // Depth-first search algorithm for finding connected components
    // Pre-condition: v must be a valid vertex, visited vector must be initialized to false
    // Post-condition: Updates visited and component vectors
    void dfs(int v, std::vector<bool>& visited, std::vector<int>& component);
};

#endif // NETWORK_H
