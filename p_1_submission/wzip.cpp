#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h> 
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "wzip: file1 [file2 ...]" << endl;
        return 1;
    }

    int fileDescriptor;
    char buffer[4096];
    int ret;

   
    char currentChar = '\0';
    int count = 0;

    for (int i = 1; i < argc; i++) {
        fileDescriptor = open(argv[i], O_RDONLY);

        if (fileDescriptor < 0) {
            cout << "wzip: cannot open file " << argv[i] << endl;
            return 1;
        }

        while ((ret = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
            for (int j = 0; j < ret; j++) {
                if (buffer[j] == currentChar) {
                    count++;
                } else {
                    if(currentChar != '\0'){
                        write(STDOUT_FILENO, &count, 4);
                        write(STDOUT_FILENO, &currentChar, 1);
                    }
                    currentChar = buffer[j];
                    count = 1;
                }
            }
        }
        close(fileDescriptor); 
    }

    if (currentChar != '\0') {
        write(STDOUT_FILENO, &count, 4);
        write(STDOUT_FILENO, &currentChar, 1);
    }

    return 0;
}
