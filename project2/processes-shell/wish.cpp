#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h> 
#include <cstdlib>




using namespace std; 

int main(int argc, char *argv[]){
    char error_message[30] = "An error has occurred\n";
    const char *path = "/bin/ls";

    char *args[] = { "ls", "-l", NULL };

    if (argc == 1) {
        char userinput[100]; 
        while(true){

            cout << "wish> "; 
            cin.getline(userinput, 100); 
            if(strcmp(userinput, "exit") == 0){
                exit(0); 
            }

            int returnCode = system(userinput);
            if(returnCode == 0){
                cout << "Is" << endl; 
            }
            
        }
        
        
    }else if(argc == 2){
        int fileDescriptor = 0;
        int ret; 
        char buffer[4096]; 

        fileDescriptor = open(argv[1], O_RDONLY);
        if (fileDescriptor < 0) {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            return 1; 
        }

        string currentLine; 
        while ((ret = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < ret; i++) {
                if (buffer[i] == '\n') {
                    
                }else {
                    currentLine += buffer[i]; 
                }
            }
            
        }

    }else{
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return 1; 
    }

    return 0; 
}