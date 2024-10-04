#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h> 

using namespace std; 

int main(int argc, char *argv[]){
    int fileDescriptor = 0;

    if (argc < 2) {
        cout << "wgrep: searchterm [file ...]" << endl;
        return 1; 
    }

    string input = argv[1]; 
    int ret;
    char buffer[4096];
    string currentLine;
    
    if (argc == 2){
        while ((ret = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < ret; i++) {
                if (buffer[i] == '\n') {
                    if (currentLine.find(input) != string::npos) {
                        cout << currentLine << endl;
                    }
                    currentLine.clear(); 
                } else {
                    currentLine += buffer[i]; 
                }
            }
        }

    } else if (argc == 3){
        fileDescriptor = open(argv[2], O_RDONLY); 
        if (fileDescriptor < 0) {
            cout << "wgrep: cannot open file" << endl;
            return 1; 
        }

        while ((ret = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < ret; i++) {
                if (buffer[i] == '\n') {
                    if (currentLine.find(input) != string::npos) {
                        cout << currentLine << endl;
                    }
                    currentLine.clear();
                } else {
                    currentLine += buffer[i];
                }
            }
        }
    }
    close(fileDescriptor); 
    
    return 0; 
}