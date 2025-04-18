#include "trie.h"

#include <cstddef>

Node::Node() : Node(nullptr, nullptr) {
}

Node::Node(uint16_t c, bool term) : Node(nullptr, nullptr, c, term) {
}

Node::Node(Node* left, Node* right) : term(false), c(0), children(std::array<Node*, 2>{left, right}), parent(nullptr) {
}

Node::Node(Node* left, Node* right, uint16_t c, bool term)
    : term(term), c(c), children(std::array<Node*, 2>{left, right}), parent(nullptr) {
}

Node::~Node() {
    delete children[0];
    delete children[1];
}

void Node::set_parent(Node* new_parent) {
    parent = new_parent;
}

Trie::Trie() : Trie(new Node()) {
}

Trie::Trie(Node* root) : root(root), now(root) {
}

Trie::~Trie() {
    now = nullptr;
    delete root;
}

void Trie::add_code(std::vector<bool> code, uint16_t c) {
    if (code.empty()) {
        return;
    }
    Node* now_copy = now;
    move_to_root();
    for (size_t i = 0; i < code.size(); ++i) {
        if (!move(code[i])) {
            now->children[static_cast<size_t>(code[i])] = new Node();
            move(code[i]);
        }
    }
    now->c = c;
    now->term = true;
    now = now_copy;
}

bool Trie::move(bool b) {
    if (now->children[static_cast<size_t>(b)] == nullptr) {
        return false;
    }
    now = now->children[static_cast<size_t>(b)];
    return true;
}

void Trie::move_to_parent() {
    now = now->parent;
}

void Trie::move_to_root() {
    now = root;
}

bool Trie::is_term() {
    if (now == nullptr) {
        return false;
    }
    return now->term;
}

uint16_t Trie::get_char() {
    if (now == nullptr) {
        return 0;
    }
    return now->c;
}