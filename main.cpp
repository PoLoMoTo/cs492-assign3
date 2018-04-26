#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <regex>
using namespace std;

struct LFileNode {
    long blockAddress;
    LFileNode* next;
};

struct LDiskNode {
    vector<long> blockIDs;
    int status; // 0 for free and 1 for used
    LDiskNode* next;
};

struct node {
    int type; // 0 for directory and 1 for file
    string name;
    node* parent;
    vector<node*> children;
    int size;
    unsigned long timestamp;
    LFileNode* LFile;
};

void filePrint(node* object){
    if (object->type == 1){
        cout << object->name << " with " << object->size << " bytes" << endl;
        cout << "Blocks: ";
        LFileNode* current = object->LFile;
        while (current != NULL){
            cout << current->blockAddress << ", ";
            current = current->next;
        }
        cout << endl;
    } else {
        for (int i = 0; i < object->children.size(); i++){
            filePrint(object->children.at(i));
        }
    }
}

int main(int argc, char const *argv[]){
    if (argc != 9)
        cout << "get rekt gg no re";

    ifstream file_list, dir_list;
    long disk_size, block_size, totalBlocks;
    string line;
    node* root = new node;
    root->name = "root";
    root->type = 0;
    root->parent = NULL;
    LDiskNode* LDiskHead = new LDiskNode;

    
    for (int i = 1; i < 9; i+=2){
        if ((string)argv[i] == "-f"){
            file_list.open(argv[i+1]);
        } else if ((string)argv[i] == "-d"){
            dir_list.open(argv[i+1]);
        } else if ((string)argv[i] == "-s"){
            disk_size = stol(argv[i+1]);
        } else if ((string)argv[i] == "-b"){
            block_size = stoi(argv[i+1]);
        }
    }

    totalBlocks = disk_size/block_size;
    
    for (int i = 0; i < totalBlocks; i++){
        LDiskHead->blockIDs.push_back(i);
    }
    LDiskHead->status = 0;
    LDiskHead->next = NULL;

    if (dir_list.is_open()){
        while (getline(dir_list, line)){
            int lastSlash = 0;
            int slash = line.find("/");
            node* parent = root;
            while (slash != string::npos){
                string dir = line.substr(lastSlash, slash-lastSlash);
                if (dir != ".") {
                    for (int i = 0; i < (*parent).children.size(); i++){
                        if (dir == parent->children.at(i)->name){
                            parent = (*parent).children.at(i);
                            break;
                        }
                    }
                }
                lastSlash = slash + 1;
                slash = line.find("/", lastSlash);
            }
            string dir = line.substr(lastSlash, line.find("\n"));
            if (dir.length() != 0) {
                for (int j = 0; j < (*parent).children.size(); j++){
                    if (dir == parent->children.at(j)->name){
                        parent = (*parent).children.at(j);
                        break;
                    }
                }
                if ((*parent).name != dir){
                    node* newDir = new node;
                    newDir->name = dir;
                    newDir->type = 0;
                    newDir->parent = parent;
                    (*parent).children.push_back(newDir);
                }
            }
        }
        dir_list.close();
    }
    cout << "Created Directory Tree" << endl;

    if (file_list.is_open()){
        while (getline(file_list, line)){
            int lastSlash = 0;
            int slash = line.find("/");
            node* parent = root;
            while (slash != string::npos){
                string dir = line.substr(lastSlash, slash-lastSlash);
                if (dir != ".") {
                    for (int i = 0; i < parent->children.size(); i++){
                        if (dir == parent->children.at(i)->name){
                            parent = parent->children.at(i);
                            break;
                        }
                    }
                }
                lastSlash = slash + 1;
                slash = line.find("/", lastSlash);
            }
            string fileName = line.substr(lastSlash, line.find("\n"));
            node* newFile = new node;
            newFile->type = 1;
            newFile->name = fileName;
            newFile->timestamp = 0;
            newFile->parent = parent;
            regex expression(R"([a-zA-Z]+\s+(\d+)\s+[a-zA-Z]{3})");
            smatch match;
            if (regex_search(line, match, expression)){
                newFile->size = stoi(match.str(1));
                LFileNode* newLL = new LFileNode;
                newLL->blockAddress = -1;
                newLL->next = NULL;
                newFile->LFile = newLL;

                if (newFile->size != 0){
                    int blocksNeeded = newFile->size/block_size;
                    if (newFile->size%block_size != 0)
                        blocksNeeded++;

                    LDiskNode* currentNode = LDiskHead;
                    LFileNode* currentFile = newFile->LFile;
                    while (currentNode != NULL){
                        if (currentNode->status == 0){
                            currentNode->status = 1;
                            if (currentNode->blockIDs.size() > blocksNeeded){
                                LDiskNode* newDiskNode = new LDiskNode;
                                newDiskNode->next = currentNode->next;
                                currentNode->next = newDiskNode;
                                newDiskNode->status = 0;
                                int sizethingpoop = currentNode->blockIDs.size();
                                for (int i = blocksNeeded; i < sizethingpoop; i++){
                                    newDiskNode->blockIDs.push_back(currentNode->blockIDs.at(blocksNeeded));
                                    currentNode->blockIDs.erase(currentNode->blockIDs.begin()+blocksNeeded);
                                }
                                for (int i = 0; i < currentNode->blockIDs.size(); i++){
                                    if (currentFile->blockAddress == -1){
                                        currentFile->blockAddress = currentNode->blockIDs.at(i);
                                    } else {
                                        LFileNode* newFileNode = new LFileNode;
                                        newFileNode->blockAddress = currentNode->blockIDs.at(i);
                                        newFileNode->next = NULL;
                                        currentFile->next = newFileNode;
                                        currentFile = currentFile->next;
                                    }
                                }
                            } else {
                                cout << "Insufficient storage space!" << endl;
                            }
                            break;
                        }
                        currentNode = currentNode->next;
                    }
                }

                parent->children.push_back(newFile);
            } else {
                cout << "No file size for " << fileName << endl;
            }
        }
        file_list.close();
    }

    cout << "Ready to receive commands" << endl;
    string command;
    node* current = root;
    while (1){
        getline(cin, command);
        if (command == "exit"){
            break;
        } else if (command == "cd.."){
            if (current->parent != NULL){
                current = current->parent;
            } else {
                cout << "No parent exists!" << endl;
            }
        } else if (command == "ls"){
            for (int i = 0; i < current->children.size(); i++){
                cout << current->children.at(i)->name << " ";
            }
            cout << endl;
        } else if (command == "dir"){
            
        } else if (command == "prfiles"){
            filePrint(root);
        } else if (command == "prdisk"){
            LDiskNode* currentDiskNode = LDiskHead;
            while (currentDiskNode != NULL){
                if (currentDiskNode->status == 0){
                    cout << "Free: " << currentDiskNode->blockIDs.at(0) << " - " << currentDiskNode->blockIDs.at(currentDiskNode->blockIDs.size()-1) << endl;
                } else {
                    cout << "In use: " << currentDiskNode->blockIDs.at(0) << " - " << currentDiskNode->blockIDs.at(currentDiskNode->blockIDs.size()-1) << endl;
                }
                currentDiskNode = currentDiskNode->next;
            }
        } else if (command.substr(0, 3) == "cd "){
            for (int i = 0; i < current->children.size(); i++){
                if (command.substr(3) == current->children.at(i)->name && current->children.at(i)->type == 1){
                    current = current->children.at(i);
                    break;
                }
            }
            if (command.substr(3) != current->name)
                cout << "Directory not found!" << endl;
        } else if (command.substr(0, 6) == "mkdir "){
            node* newDir = new node;
            newDir->name = command.substr(6);
            newDir->type = 0;
            newDir->parent = current;
            current->children.push_back(newDir);
        } else if (command.substr(0, 7) == "create "){
            node* newFile = new node;
            newFile->type = 1;
            newFile->name = command.substr(7);
            newFile->timestamp = 0;
            newFile->parent = current;
            newFile->size = 0;
            LFileNode* newLL = new LFileNode;
            newLL->blockAddress = -1;
            newLL->next = NULL;
            newFile->LFile = newLL;
            current->children.push_back(newFile);
        } else if (command.substr(0, 4) == "add "){
            string fileName = command.substr(4, command.find(' ', 4)-4);
            long bytes = stol( command.substr( command.find(' ', 4) ) );
            cout << fileName << ", " << bytes << endl;

            int blocksNeeded = bytes/block_size;
            if (bytes%block_size != 0)
                blocksNeeded++;

            LDiskNode* currentNode = LDiskHead;
            node* currentFile;
            for (int i = 0; i < current->children.size(); i++){
                if (current->children.at(i)->name == fileName)
                    currentFile = current->children.at(i);
            }
            LFileNode* currentLFile = currentFile->LFile;
            while (currentNode != NULL){
                if (currentNode->status == 0){
                    currentNode->status = 1;
                    if (currentNode->blockIDs.size() > blocksNeeded){
                        LDiskNode* newDiskNode = new LDiskNode;
                        newDiskNode->next = currentNode->next;
                        currentNode->next = newDiskNode;
                        newDiskNode->status = 0;
                        int sizethingpoop = currentNode->blockIDs.size();
                        for (int i = blocksNeeded; i < sizethingpoop; i++){
                            newDiskNode->blockIDs.push_back(currentNode->blockIDs.at(blocksNeeded));
                            currentNode->blockIDs.erase(currentNode->blockIDs.begin()+blocksNeeded);
                        }
                        for (int i = 0; i < currentNode->blockIDs.size(); i++){
                            if (currentLFile->blockAddress == -1){
                                currentLFile->blockAddress = currentNode->blockIDs.at(i);
                            } else {
                                LFileNode* newFileNode = new LFileNode;
                                newFileNode->blockAddress = currentNode->blockIDs.at(i);
                                newFileNode->next = NULL;
                                currentLFile->next = newFileNode;
                                currentLFile = currentLFile->next;
                            }
                        }
                        blocksNeeded = 0;
                    } else {
                        int sizethingpoop = currentNode->blockIDs.size();
                        for (int i = blocksNeeded; i < sizethingpoop; i++){
                            if (currentLFile->blockAddress == -1){
                                currentLFile->blockAddress = currentNode->blockIDs.at(blocksNeeded);
                            } else {
                                LFileNode* newFileNode = new LFileNode;
                                newFileNode->blockAddress = currentNode->blockIDs.at(blocksNeeded);
                                newFileNode->next = NULL;
                                currentLFile->next = newFileNode;
                                currentLFile = currentLFile->next;
                            }
                        }
                        blocksNeeded -= currentNode->blockIDs.size();
                    }
                    if (blocksNeeded <= 0)
                        break;
                }
                currentNode = currentNode->next;
            }

        } else if (command.substr(0, 7) == "remove "){
            string fileName = command.substr(7, command.find(' ', 7)-7);
            long bytes = stol( command.substr( command.find(' ', 7) ) );
            cout << fileName << ", " << bytes << endl;
        } else {
            cout << "Unknown command" << endl;
        }
    }
}