#include <iostream>

using std::cout;
using std::endl;

template <typename T, typename V>
class Node {
public:
    Node();
    Node(K key, V value, int);

    K getKey() const;
    V getVaue() const;
    void setValue(V value);
    Node<K, V> **forward;
    int nodeLevel;

    ~Node();

private:
    K _key;
    V _value;
};