#include <array>
#include <cstdint>
#include <vector>

struct Node {
    bool term;
    uint16_t c;
    std::array<Node*, 2> children;
    Node* parent;

    Node();

    Node(uint16_t c, bool term);

    Node(Node* left, Node* right);

    Node(Node* left, Node* right, uint16_t c, bool term);

    ~Node();

    void set_parent(Node* parent);
};

class Trie {
public:
    Trie();

    Trie(Node* root);

    ~Trie();

    void add_code(std::vector<bool> code, uint16_t c);

    bool move(bool b);

    void move_to_parent();

    void move_to_root();

    bool is_term();

    uint16_t get_char();

private:
    Node* root;
    Node* now;
};