#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace std; 

int main(int argc, char *argv[]){
    
    int fileDescriptor;

    if (argc < 2) {
        return 0; 
    }
    
    int ret;
    char buffer[4096];
    
    for(int i = 1; i < argc; i++){
        fileDescriptor = open(argv[i], O_RDONLY);
        if (fileDescriptor < 1) {
            cout << "wcat: cannot open file" << endl;
            return 1; 
        }

        while ((ret = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
            if(write(STDOUT_FILENO, buffer, ret) == -1){
                return 1; 
            } 
        }
        
        close(fileDescriptor);
    }
    return 0; 
}