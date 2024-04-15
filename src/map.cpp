#ifndef MAP_F

#define MAP_F
#include "debug.cpp"
#include <bits/stdc++.h>
#include <stdexcept>

/*
 * Known Issues:
 * 1. Floating point keys are not handled properly.
 * 2. The [] operator does not handle the case when the key is not found
 *properly, throws a runtime error instead of returning a default value.
 */

template <typename K, typename V>
struct Node {
    K key;
    V value;
    Node *next;

    Node(K key, V value) : key(key), value(value), next(nullptr) {}
};

template <typename K, typename V>
class UnorderedMap {
    Node<K, V> **buckets;
    int num_buckets;
    int size;

    // Hash function for types other than string,casts to int for functionality

    void rehash() {
        Node<K, V> **old_buckets = buckets;
        int old_num_buckets = num_buckets;

        num_buckets *= 2;
        size = 0;
        buckets = new Node<K, V> *[num_buckets];

        for (int i = 0; i < old_num_buckets; ++i) {
            Node<K, V> *node = old_buckets[i];
            while (node) {
                insert(node->key, node->value);
                Node<K, V> *old_node = node;
                node = node->next;
                delete old_node;
            }
        }

        delete[] old_buckets;
    }

  public:
    UnorderedMap(int num_buckets = 16) : num_buckets(num_buckets), size(0) {
        buckets = new Node<K, V> *[num_buckets];
    }

    ~UnorderedMap() {
        for (int i = 0; i < num_buckets; ++i) {
            Node<K, V> *node = buckets[i];
            while (node) {
                Node<K, V> *next_node = node->next;
                delete node;
                node = next_node;
            }
        }
        delete[] buckets;
    }

    void insert(K key, V value) {
        int index = std::hash<K>{}(key) % num_buckets;
        Node<K, V> *node = buckets[index];
        while (node) {
            if (node->key == key) {
                node->value = value;
                return;
            }
            node = node->next;
        }

        ++size;
        if (size > num_buckets / 2) {
            rehash();
        }

        node = new Node<K, V>(key, value);
        node->next = buckets[index];
        buckets[index] = node;
    }

    // returns all elements in the map in a linked list
    Node<K, V> *getElements() {
        Node<K, V> *all_elements = nullptr; // Head of the linked list

        // Traverse all buckets
        for (int i = 0; i < num_buckets; ++i) {
            Node<K, V> *node = buckets[i];
            // Traverse each chain in the bucket
            while (node) {
                Node<K, V> *new_node = new Node<K, V>(node->key, node->value);
                new_node->next = all_elements;
                all_elements = new_node;
                node = node->next;
            }
        }

        return all_elements;
    }
    V operator[](K key) {
        int index = std::hash<K>{}(key) % num_buckets;
        Node<K, V> *node = buckets[index];
        while (node) {
            if (node->key == key) {
                return node->value;
            }
            node = node->next;
        }
        debug("Key not found");
        throw std::runtime_error("Key not found");
    }

    void erase(K key) {
        int index = std::hash<K>{}(key) % num_buckets;
        Node<K, V> *node = buckets[index];
        if (node && node->key == key) {
            buckets[index] = node->next;
            delete node;
            size--;
            return;
        }

        while (node) {
            if (node->next && node->next->key == key) {
                Node<K, V> *old_node = node->next;
                node->next = old_node->next;
                delete old_node;
                size--;
                return;
            }
            node = node->next;
        }
    }
};

// specialised class definition for string
template <typename V>
class UnorderedMap<std::string, V> {
    Node<std::string, V> **buckets;
    int num_buckets;
    int size;


    void rehash() {
        Node<std::string, V> **old_buckets = buckets;
        int old_num_buckets = num_buckets;

        num_buckets *= 2;
        size = 0;
        buckets = new Node<std::string, V> *[num_buckets];

        for (int i = 0; i < old_num_buckets; ++i) {
            Node<std::string, V> *node = old_buckets[i];
            while (node) {
                insert(node->key, node->value);
                Node<std::string, V> *old_node = node;
                node = node->next;
                delete old_node;
            }
        }

        delete[] old_buckets;
    }

  public:
    UnorderedMap(int num_buckets = 32) : num_buckets(num_buckets), size(0) {
        buckets = new Node<std::string, V> *[num_buckets];
        for (int i = 0; i < num_buckets; ++i) {
            buckets[i] = nullptr;
        }
    }

    ~UnorderedMap() {
        for (int i = 0; i < num_buckets; ++i) {
            Node<std::string, V> *node = buckets[i];
            while (node) {
                Node<std::string, V> *next_node;
                next_node = node->next;
                delete node;
                node = next_node;
            }
        }
        delete[] buckets;
    }

    void insert(std::string key, V value) {
        int index = std::hash<std::string>{}(key) % num_buckets;
        Node<std::string, V> *node = buckets[index];
        while (node) {
            if (node->key == key) {
                node->value = value;
                return;
            }
            node = node->next;
        }

        ++size;
        if (size > num_buckets / 2) {
            rehash();
        }

        node = new Node<std::string, V>(key, value);
        node->next = buckets[index];
        buckets[index] = node;
    }

    Node<std::string, V> *getElements() {
        Node<std::string, V> *all_elements = nullptr; // Head of the linked list

        // Traverse all buckets
        for (int i = 0; i < num_buckets; ++i) {
            Node<std::string, V> *node = buckets[i];
            // Traverse each chain in the bucket
            while (node) {
                // Create a new node for the linked list
                Node<std::string, V> *new_node = new Node<std::string, V>(node->key, node->value);
                // Insert the new node at the beginning of the linked list
                new_node->next = all_elements;
                all_elements = new_node;
                // Move to the next node in the chain
                node = node->next;
            }
        }

        return all_elements;
    }
    V operator[](std::string key) {
        int index = std::hash<std::string>{}(key) % num_buckets;
        Node<std::string, V> *node = buckets[index];
        while (node) {
            if (node->key == key) {
                return node->value;
            }
            node = node->next;
        }
        debug("Key not found");
        return V();
        // throw std::runtime_error("Key not found");
    }

    void clear() {
        for (int i = 0; i < num_buckets; ++i) {
            Node<std::string, V> *node = buckets[i];
            while (node) {
                Node<std::string, V> *next_node = node->next;
                delete node;
                node = next_node;
            }
            buckets[i] = nullptr;
        }
        size = 0;
    }

    void erase(std::string key) {
        int index = std::hash<std::string>{}(key) % num_buckets;
        Node<std::string, V> *node = buckets[index];
        if (node && node->key == key) {
            buckets[index] = node->next;
            delete node;
            size--;
            return;
        }

        while (node) {
            if (node->next && node->next->key == key) {
                Node<std::string, V> *old_node = node->next;
                node->next = old_node->next;
                delete old_node;
                size--;
                return;
            }
            node = node->next;
        }
    }
};
#endif

/* int main() {
    UnorderedMap<int, std::string> map;

    map.insert(1, "one");
    map.insert(2, "two");
    map.erase(2);
    map.insert(3, "three");

    try {
        std::cout << "1: " << map[1] << std::endl;
        std::cout << "2: " << map[2] << std::endl;
        std::cout << "3: " << map[3] << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
} */
