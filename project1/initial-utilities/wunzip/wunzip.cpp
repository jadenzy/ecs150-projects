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
        cout << "wunzip: file1 [file2 ...]" << endl;
        return 1;
    }

    int fileDescriptor;
    int count;
    char character;

    for (int i = 1; i < argc; i++) {
        fileDescriptor = open(argv[i], O_RDONLY);

        if (fileDescriptor < 0) {
            cout << "wunzip: cannot open file " << argv[i] << endl;
            return 1;
        }
        int ret; 
        while ((ret = read(fileDescriptor, &count, 4) > 0)) {
            if (read(fileDescriptor, &character, 1) > 0) {
                for (int j = 0; j < count; j++) {
                    write(STDOUT_FILENO, &character, 1);
                }
            } else {
                close(fileDescriptor);
                return 1;
            }
        }

        close(fileDescriptor);
    }

    return 0;
}
