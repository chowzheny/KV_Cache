#include "skipList.hh"

#include <cstring>
#include <mutex>

std::mutex mtx;


// 构造函数
template <typename K, typename V>
Node<K, V>::Node(const K key, const V value, int level) 
:_key(key)
,_value(value)
{
    this->nodeLevel = level;

    // 指针数组，长度为level + 1，用于存储指向其他节点的指针
    // forward是指节点在这一层对应的下一个节点
    this->forward = new Node<K, V> *[level + 1];  

    memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
}

template <typename K, typename V>
Node<K, V>::~Node() {
    delete [] forward;
}

template <typename K, typename V>
K Node<K, V>::getKey() const {
    // cout << "getKey()" << endl;
    return _key;
}
template <typename K, typename V>
V Node<K, V>::getVaue() const {
    // cout << "getValue()" << endl;
    return _value;
}

template <typename K, typename V>
void Node<K, V>::setValue(V value) {
    this->_value = value;
}


/*
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    *
    
*/

template <typename K, typename V>
skipList<K, V>::skipList(int maxLevel)
:_maxLevel(maxLevel)
,_skipListLevel(0)
,_elemCount(0)
{
    // 初始化头节点
    K key;
    V value;
    this->_header = new Node<K, V>(key, value, _maxLevel);
}

template<typename K, typename V>
skipList<K, V>::~skipList() {
    if(_fileWriter.is_open()) {
        _fileWriter.close();
    }
    if(_fileReader.is_open()) {
        _fileReader.close();
    }

    delete _header;
}

/*

    跳表的功能函数

*/
// 创建新的节点
template<typename K, typename V>
Node<K, V> * skipList<K, V>::createNode(const K key, const V value, int level) {
    Node<K, V> *n = new Node<K, V>(key, value, level);
    return n;
}

// 插入新的节点，插入成功返回0，节点已经存在返回1
template <typename K, typename V>
int skipList<K, V>::insertElement(const K key, const V value){
    mtx.lock();

    Node<K, V> *current = this->_header;

    Node<K, V> *update[_maxLevel + 1];  // 记录每一层要插在哪一个节点后
    memset(update, 0, sizeof(Node<K, V>*) * (_maxLevel + 1));

    /*
        从当前层开始向下层开始遍历，找到每一层最后一个小于 key 值的节点
        用update存储每层的这样的节点，
    */
    for (int i = _skipListLevel; i>= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->getKey() < key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 回到第0层的插入位置
    current = current->forward[0];

    // 节点存在了就解锁，返回 1
    if (current != nullptr && current->getKey() == key) {
        cout << "key: " << key << ", already exits!" << endl;
        mtx.unlock();
        return 1;
    }

    if (current == nullptr || current->getKey() != key) {

        // 生成一个随机的层
        int randomLevel = getRandomLevel();

        // 随机生成的层 > 当前层，那么在当前层之上，新节点将被插入到_header节点后面
        if (randomLevel > _skipListLevel) {
            for (int i = _skipListLevel + 1; i < randomLevel + 1; i++) {
                update[i] = _header;
            }
            _skipListLevel = randomLevel;
        }

        // 新的节点
        Node<K, V> *insertNode = createNode(key, value, randomLevel);

        // 插入操作
        for (int i = 0; i <= randomLevel; i++) {
            insertNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insertNode;
        }

        cout << "Successfully inserted key: " << key << ", value:" << value << endl;
        _elemCount ++;
    }

    mtx.unlock();
    return 0;
}

template<typename K, typename V>
int skipList<K, V>::getRandomLevel() {
    int k = 1;
    while (rand() % 2 == 0) {
        k++;
    }
    k = (k < _maxLevel) ? k : _maxLevel;
    return k;
}

template<typename K, typename V>
void skipList<K, V>::dumpFile() {
    cout << "\n----------------dump file ---------------------" << endl;
    _fileWriter.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0];

    // 只用遍历第一层节点就行
    while (node != nullptr) {
        _fileWriter << node->getKey() << ":\t\t" << node->getVaue() << '\n';
        cout << node->getKey() << ":\t" << node->getVaue() << ";\n";
        node = node->forward[0];
    }

    _fileWriter.flush();
    _fileWriter.close();
}

// 从文件中读取数据存入数据库
template<typename K, typename V>
void skipList<K, V>::loadFile() {
    _fileReader.open(STORE_FILE);  
    cout << "\n----------------load file ---------------------" << endl;
    string line;
    string *key = new string();
    string *value = new string();
    while (getline(_fileReader, line)) {
        getKeyValue_fromString(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insertElement(*key, *value);
        cout << "key: " << *key << "value:" << *value << endl;
    }
    delete key;
    delete value;
    key = nullptr;
    value = nullptr;
    _fileReader.close();
}

template<typename K, typename V>
void skipList<K, V>::getKeyValue_fromString(const string &str, string *key, string *value) {
    if (!isValidString(str)) {
        return;
    }

    *key = str.substr(0, str.find(":"));
    *value = str.substr(str.find(":") + 1, str.size());
}


template<typename K, typename V>
bool skipList<K, V>::isValidString(const string &str) {
    /*
        npos 是一个静态成员变量，它是 std::string 类型的一个特殊值。
        它表示一个无效的或未找到的字符串位置，通常用于表示字符串查找失败的情况。

        find() 函数未找到所需子串时，它会返回 std::string::npos，表示未查找到。
    */
    if (str.empty()) {
        return false;
    }
    if (str.find(":") == string::npos) {
        return false;
    }
    return true;
}

template<typename K, typename V>
int skipList<K, V>::size() {
    return _elemCount;
}

template<typename K, typename V>
void skipList<K, V>::printList() {
    cout << "\n-----------skipList-----------------" << endl;
    for (int i = 0; i <= _skipListLevel; i++) {
        Node<K, V> *node = _header->forward[i];
        cout << "Level " << i << " : ";
        while (node != nullptr) {
            cout << node->getKey() << ":" << node->getVaue() << ";";
            node = node->forward[i];
        }
        cout << endl;
    }
}

template<typename K, typename V>
bool skipList<K, V>::searchNode(K key) {
    cout << "serach Node: -----------------" << endl;
    Node<K, V> *current = _header;

    // 找到每层最后一个小于key的节点
    for (int i = _skipListLevel; i>= 0; i --) {
        while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
    }

    // 节点最终都是插入到0层的链表中
    // 至始至终都只有一条链表，其他层的都是以下标访问
    current = current->forward[0];
    if (current != nullptr && current->getKey() == key) {
        cout << "Found node->(" << key << ", " << current->getVaue() << ")" << endl;
        return true;
    }

    cout << "Not found node " << key << endl;
    return false;
}

template<typename K, typename V>
void skipList<K, V>::deleteNode(K key) {
    mtx.lock();

    Node<K, V> *current = _header;
    Node<K, V>  *update[_maxLevel + 1];
    memset(update, 0, sizeof(Node<K, V>  *) * (_maxLevel + 1));

    for (int i = _skipListLevel; i >= 0; i--) {
        if (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->getKey() == key) {

        // 从下往上找目标节点，如果该层没有节点就break
        for (int i = 0; i <= _skipListLevel; i++) {
            if (update[i]->forward[i] != current) {
                break;
            }
            
            update[i]->forward[i] = current->forward[i];
        }

        // 删除了节点后，删减层数
        // 该层的 _header 后没有节点
        while (_skipListLevel > 0 && _header->forward[_skipListLevel] == 0) {
            _skipListLevel --;
        }

        cout << "Successfully deleted node: (" << key << ", " << current->getVaue() << ")" << endl;
        delete current;
        current = nullptr;
        _elemCount --;
    }
    mtx.unlock();
    return;
}
