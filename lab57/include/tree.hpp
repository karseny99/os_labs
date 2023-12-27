#pragma once

#include <memory>
#include <set>
#include <string>
#include <iostream>


class node {

public:
    int id;
    node * child = nullptr;
    node * brother = nullptr;

    bool is_available;
    std::set<int> controlled_ids;

    node(int _id) : id(_id) {
        child = nullptr;
        brother = nullptr;
        is_available = true;
    };

    node * get_node(const std::string& path) {
        node * found_node = move_to_node(this, path, 0);
        return found_node;
    }

    static node *  move_to_node(node *  current_node, const std::string& path, int idx) {
        if (idx >= path.size()) {
            return current_node;
        }
        else if (path[idx] == 'c' && current_node != nullptr) {
            return move_to_node(current_node->child, path, idx + 1);
        }
        else if (path[idx] == 'b' && current_node != nullptr) {
            return move_to_node(current_node->brother, path, idx + 1);
        }
        else {
            return nullptr;
        }
    }

    bool add_node(std::string path, int id) {
        node * existed_node = move_to_node(this, path, 0);

        if(existed_node == nullptr) {
            return false;
        }

        node * new_node = new node(id); 
        new_node->brother = nullptr;
        new_node->child = nullptr;

        node * move_ptr = existed_node->child;
        if(move_ptr != nullptr) {
            while(move_ptr->brother != nullptr) {
                move_ptr = move_ptr->brother;
            }
            move_ptr->brother = new_node;
        } else {
            existed_node->child = new_node;
        }
        return true;
    }

    bool exist(int id) {
        return controlled_ids.find(id) != controlled_ids.end();
    }
};