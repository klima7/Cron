#include <string>
#include <vector>

using namespace std;

int interpret(vector<string> arguments) {
    if(arguments.size() == 0) {
        printf("Starting command");
    }

    return 0;
}

int main(int argc, char **argv) {
    vector<string> arguments;
    for(int i=1; i<argc; i++)
        arguments.emplace_back(argv[0]);

    return interpret(arguments);
}