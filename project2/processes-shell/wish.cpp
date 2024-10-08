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
#include <map>

using namespace std; 

string remove_whitespace(string &str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    if (first == string::npos){
        return NULL; 
    }
    return str.substr(first, last - first + 1);
}

vector<string> split(string &input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, '&')) {
        string command = remove_whitespace(token);
        if (!command.empty()) { 
            tokens.push_back(command);
        }
    }
    return tokens;
}


int main(int argc, char *argv[]){
    char error_message[30] = "An error has occurred\n";

    vector<char*> paths;
    char default_path[100] = "/bin"; 
    paths.push_back(default_path); 

    string user_input; 

    if (argc == 1) {
        while(true){
            cout << "wish> "; 
            getline(cin, user_input); 

            user_input = remove_whitespace(user_input);
            if (user_input.empty()) {
                continue; 
            }

            vector<string> commands = split(user_input);

            for (const auto &command : commands) {
                vector<char*> args;
                stringstream ss(command);
                string arg;
                ss >> arg;
                if(arg == "exit"){
                    exit(0); 
                }else if(arg == "path"){
                    while (ss >> arg) {
                        paths.push_back(strdup(arg.c_str()));  
                    }
                }else if(arg == "cd"){
                    int argNum = 0;
                    while (ss >> arg) {
                        argNum++;   
                    }
                    if(argNum == 1){
                        char* newDirectory = strdup(arg.c_str());
                        if (chdir(newDirectory) == -1){
                            write(STDERR_FILENO, error_message, strlen(error_message)); 
                        }
                    }else{
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                    }
                }else{
                    for (const auto &path : paths){
                        size_t new_size = strlen(path) + 2; 
                        char *new_path = new char[new_size];
                        strcpy(new_path, path);
                        strcat(new_path, "/");
                        string full_path = new_path + arg;
                        
                        if(access(path, X_OK) == 0){
                            args.push_back(strdup(full_path.c_str()));
                            while (ss >> arg) {
                                args.push_back(strdup(arg.c_str()));  
                            }
                            args.push_back(nullptr);

                            pid_t pid = fork();

                            if (pid == 0) {
                                if (execv(args[0], args.data()) == -1) {
                                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                                }
                            } else if (pid > 0) {
                                wait(NULL);
                            } else {
                                write(STDERR_FILENO, error_message, strlen(error_message)); 
                            }
                            break; 
                        }
                    }
                }
            }
        }
    // }else if(argc == 2){
    //     int fileDescriptor = 0;
    //     int ret; 
    //     char buffer[4096]; 

    //     fileDescriptor = open(argv[1], O_RDONLY);
    //     if (fileDescriptor < 0) {
    //         write(STDERR_FILENO, error_message, strlen(error_message)); 
    //         return 1; 
    //     }

    //     string currentLine; 
    //     while ((ret = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
    //         for (int i = 0; i < ret; i++) {
    //             if (buffer[i] == '\n') {
                    
    //             }else {
    //                 currentLine += buffer[i]; 
    //             }
    //         }
            
    //     }

    // }else{
    //     write(STDERR_FILENO, error_message, strlen(error_message)); 
    //     return 1; 
    }

    return 0; 
}