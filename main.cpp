#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
using namespace std;


int main(int argc, char const *argv[]){

    if (argc != 9)
        cout << "get rekt gg no re";

    ifstream file_list, dir_list;
    int disk_size, block_size;
    
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
}