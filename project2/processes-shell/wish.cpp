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

char error_message[30] = "An error has occurred\n";
vector<char*> paths;

string remove_whitespace(string &str) {
    int first = str.find_first_not_of(" \t");
    int last = str.find_last_not_of(" \t");
    if (first == string::npos) return ""; 
    return str.substr(first, last - first + 1);
}

bool detect_redirection(string &arg, string &command, string &output_file){
    size_t pos = arg.find('>');
    if (pos != string::npos){
        command = arg.substr(0, pos); 
        command = remove_whitespace(command); 
        output_file = arg.substr(pos + 1); 
        output_file = remove_whitespace(output_file); 
        return true;
    }
    return false;
}

vector<string> split(string &input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, '&')) {
        string command = remove_whitespace(token);
        if (!command.empty()) tokens.push_back(command);
    }
    return tokens;
}

void excu(vector<string> commands) {
    vector<pid_t> pids;
    for (const auto &command : commands) {
        vector<char*> args;
        stringstream ss(command);
        string arg;
        string first_arg; 
        ss >> arg;
        first_arg = arg; 
        string red_command;
        string out_file; 
        if(arg[0] == '>') {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            continue;
        }
        
        if(arg == "exit"){
            vector<string> exit_args;
            while (ss >> arg) {
                exit_args.push_back(arg);   
            }
            if(exit_args.size() > 0){
                write(STDERR_FILENO, error_message, strlen(error_message)); 
            }
            else{
                exit(0); 
            }
        } else if(arg == "path"){
            paths.clear();
            while (ss >> arg) {
                if (find(paths.begin(), paths.end(), strdup(arg.c_str())) == paths.end()) {
                    paths.push_back(strdup(arg.c_str()));
                }
            }
        } else if(arg == "cd"){
            vector<string> cd_args;
            while (ss >> arg) {
                cd_args.push_back(arg);   
            }
            if (cd_args.size() == 1) {
                if (chdir(cd_args[0].c_str()) == -1) { 
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                }
            } else {
                write(STDERR_FILENO, error_message, strlen(error_message)); 
            }
        } 
        else {
            bool redirect = false;
            string output_file;
            vector<string> args_list;
            red_command.clear();
            out_file.clear(); 
            while (ss >> arg) {
                if (detect_redirection(arg, red_command, out_file)) {
                    stringstream of(out_file); 
                    if(red_command != ""){
                        args_list.push_back(red_command); 
                    }
                    redirect = true;
                    if(out_file == ""){ 
                        int file_count = 0;
                        while (ss >> output_file) {
                            file_count++;
                        }

                        if (file_count != 1) {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);  
                        }
                        break; 
                    }

                    int file_count = 0;
                    while (of >> output_file) {
                        file_count++;
                    }

                    if (file_count != 1) {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(0);  
                    }
                    break;  
                } else {
                    args_list.push_back(arg);  
                }
            }

            string full_path;
            bool command_found = false;
            for (const auto &path : paths) {
                full_path = string(path);
                if (full_path.back() != '/') {
                    full_path += '/';
                }
                full_path += first_arg;

                if (access(full_path.c_str(), X_OK) == 0) {
                    command_found = true;
                    break;
                }
            }

            if (!command_found) {
                write(STDERR_FILENO, error_message, strlen(error_message)); 
                continue;
            }
            
            args.push_back(strdup(full_path.c_str()));
            for (int i = 0; i < args_list.size(); i++) {
                
                args.push_back(strdup(args_list[i].c_str()));  
            }
            args.push_back(nullptr);

            pid_t pid = fork();
            
            if (pid == 0) {
                if (redirect) {
                    int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                    if (dup2(fd, STDOUT_FILENO) == -1 || fd == -1)
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                    close(fd);
                }
                if (execv(args[0], args.data()) == -1) 
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                exit(1);
            } else if (pid > 0) {
                pids.push_back(pid);
                
            } else {
                write(STDERR_FILENO, error_message, strlen(error_message)); 
            }
        }
        
    }
    for (pid_t pid : pids) {
        waitpid(pid, nullptr, 0);
    }
}

int main(int argc, char *argv[]){
    char default_path[100] = "/bin"; 
    paths.push_back(default_path);
    
    if (argc == 1) {
        string user_input; 
        while(true){
            cout << "wish> "; 
            getline(cin, user_input); 
            user_input = remove_whitespace(user_input);
            if (user_input.empty()) {
                continue; 
            }
            vector<string> commands = split(user_input);
            excu(commands);
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
                    currentLine = remove_whitespace(currentLine);
                    if (!currentLine.empty()) {
                        vector<string> commands = split(currentLine);
                        excu(commands);
                    }
                    currentLine.clear(); 
                }else {
                    currentLine += buffer[i]; 
                }
            }
            close(fileDescriptor); 
        }
    }else{
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return 1; 
    }
    return 0; 
}