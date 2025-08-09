#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./sha256 [Message]\n";
        
        return 1;
    }

    std::cout << "Message: " << argv[1] << '\n';

    /*
        Steps to produce some hash!

        1. Convert message to binary!

        2. Preprocessing
        2.1 Padding the message

        3. Parse the message

        4. 
    */

    return 0;
}