#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h> 
#include <sstream> 
#include <cstdlib>
#include <vector>  

using namespace std; 

int main(int argc, char *argv[]){
    int fileDescriptor;
    
    int ret;
    char buffer[4096];
    vector<pid_t> pids; 
    for(int i = 1; i < argc; i++){
        pid_t pid = fork(); 
        if(pid == 0){
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
        }else if (pid > 0){
            pids.push_back(pid);
        }else{
            return 1; 
        }
        for (pid_t pid : pids) {
            waitpid(pid, nullptr, 0);
        }
    }
    return 0; 
}