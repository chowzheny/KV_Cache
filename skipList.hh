#pragma once
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::string;

#define STORE_FILE "store/dumpFile"

template <typename K, typename V>
class Node {
public:
    Node();
    Node(K key, V value, int level);

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

template <typename K, typename V>
class skipList {
public:
    skipList(int);
    ~skipList();

    Node<K, V>* createNode(K, V, int);
    int insertElement(K, V);
    int getRandomLevel();
    void dumpFile();
    void loadFile();
    int size();
    void printList();
    bool searchNode(K);
    void deleteNode(K);
private:
    void getKeyValue_fromString(const string &str, string *key, string *value);
    bool isValidString(const string &str);
private:
    int _elemCount;
    Node<K, V> *_header;
    int _maxLevel;  // 最大层
    int _skipListLevel;  // 当前层

    ofstream _fileWriter;
    ifstream _fileReader;
};