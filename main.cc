#include "skipList.hh"
#include "skipList.cc"
#include <string>

using std::string;

void test () {
    skipList<int, string> skiplist(6);
    skiplist.insertElement(1, "niko");
    skiplist.insertElement(3, "simple");
    skiplist.insertElement(7, "karirr");
    skiplist.insertElement(8, "ele");
    skiplist.insertElement(9, "perfecto");
    skiplist.insertElement(19, "zywoo");
    skiplist.insertElement(19, "apex");

    cout << "skipList's size = " << skiplist.size() << endl;
    skiplist.dumpFile();

    skiplist.searchNode(9);
    skiplist.searchNode(18);

    skiplist.printList();

    skiplist.deleteNode(2);
    skiplist.deleteNode(3);

    cout << "skipList's size = " << skiplist.size() << endl;
    skiplist.printList();
}

int main() {
    test();
}