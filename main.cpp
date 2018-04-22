#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <regex>
using namespace std;

struct LFileNode {
    int startAddress;
    LFileNode* next;
};

struct LDiskNode {
    vector<int> blockIDs;
    int status; // 0 for free and 1 for used
    LDiskNode* next;
};

struct node {
    int type; // 0 for directory and 1 for file
    string name;
    vector<node> children;
    int size;
    unsigned long timestamp;
    LFileNode LFile;
};

int main(int argc, char const *argv[]){

    if (argc != 9)
        cout << "get rekt gg no re";

    ifstream file_list, dir_list;
    int disk_size, block_size, totalBlocks;
    string line;
    node root;
    root.name = "root";
    root.type = 0;
    LDiskNode LDiskHead;
    
    for (int i = 1; i < 9; i+=2){
        if ((string)argv[i] == "-f"){
            file_list.open(argv[i+1]);
        } else if ((string)argv[i] == "-d"){
            dir_list.open(argv[i+1]);
        } else if ((string)argv[i] == "-s"){
            disk_size = stoi(argv[i+1]);
        } else if ((string)argv[i] == "-b"){
            block_size = stoi(argv[i+1]);
        }
    }

    totalBlocks = disk_size/block_size;
    
    for (int i = 0; i < totalBlocks; i++){
        LDiskHead.blockIDs.push_back(i);
    }
    LDiskHead.status = 0;
    cout << LDiskHead.blockIDs.at(5) << endl;

    if (dir_list.is_open()){
        while (getline(dir_list, line)){
            int lastSlash = 0;
            int slash = line.find("/");
            node parent = root;
            while (slash != string::npos){
                string dir = line.substr(lastSlash, slash-lastSlash);
                if (dir != ".") {
                    for (vector<node>::iterator it = parent.children.begin(); it != parent.children.end(); ++it){
                        if (dir == (*it).name){
                            parent = *it;
                            break;
                        }
                    }
                    if (parent.name != dir){
                        node newDir;
                        newDir.name = dir;
                        newDir.type = 0;
                        parent.children.push_back(newDir);
                        parent = newDir;
                    }
                }
                lastSlash = slash + 1;
                slash = line.find("/", lastSlash);
            }
            string dir = line.substr(lastSlash, line.find("\n"));
            if (dir.length() != 0) {
                for (vector<node>::iterator it = parent.children.begin(); it != parent.children.end(); ++it){
                    if (dir == (*it).name){
                        parent = *it;
                        break;
                    }
                }
                if (parent.name != dir){
                    node newDir;
                    newDir.name = dir;
                    newDir.type = 0;
                    parent.children.push_back(newDir);
                }
            }
        }
        dir_list.close();
    }

    if (file_list.is_open()){
        while (getline(file_list, line)){
            int lastSlash = line.find("/");
            int slash = line.find("/", lastSlash + 1);
            node parent = root;
            while (slash != string::npos){
                string dir = line.substr(lastSlash, slash-lastSlash);
                for (vector<node>::iterator it = parent.children.begin(); it != parent.children.end(); ++it){
                    if (dir == (*it).name){
                        parent = *it;
                        break;
                    }
                }
                lastSlash = slash + 1;
                slash = line.find("/", lastSlash);
            }
            string fileName = line.substr(lastSlash, line.find("\n"));
            node newFile;
            newFile.type = 1;
            newFile.name = fileName;
            newFile.timestamp = 0;
            regex expression(R"([a-zA-Z]+\s+(\d+)\s+[a-zA-Z]{3})");
            smatch match;
            if (regex_search(line, match, expression)){
                newFile.size = stoi(match.str(1));
                parent.children.push_back(newFile);
            } else {
                cout << "No file size for " << fileName << endl;
            }
        }
        file_list.close();
    }

    // cout << root->name << ": " << endl;
    // for (vector<node*>::iterator it = root->children.begin(); it != root->children.end(); ++it){
    //     cout << (*it)->name << ": ";
    //    /*  for (vector<node*>::iterator itt = (*it)->children.begin(); itt != (*it)->children.end(); ++itt){
    //         cout << (*itt)->name << ": ";
    //         for (vector<node*>::iterator ittt = (*itt)->children.begin(); ittt != (*itt)->children.end(); ++ittt){
    //             cout << (*ittt)->name << ", ";
    //         }
    //         cout << endl;
    //     } */
    //     cout << endl;
    // }
}