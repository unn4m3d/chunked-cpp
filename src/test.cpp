#include <iostream>
#include <fstream>
#include <iomanip>
#include "chunked.hpp"
using namespace std;
using chunked::reader;

int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        cerr << "File name is not specified" << endl;
        exit(1);
    }
    cout << "Starting read test" << endl;
    ifstream input(argv[1], fstream::binary);
    
    reader<size_t,size_t> rd(&input);
    auto c = rd.open_chunk();
    cout << "Opened chunk #" << c.get_index() << "(" << c.get_size() << ")" << endl;
    auto cur = rd.current_chunk(c);
    cout << "Offset : " << cur.offset << endl;
    cout << "======[DATA]======" << endl;
    for(size_t i = 0; i < cur.buffer.size(); ++i)
    {
        if(i > 0 && i % 16 == 0)
        {
            cout << endl;
        }
        cout << hex << setfill('0') << setw(3) << cur.buffer[i] << " ";
    }
    cout << endl << "==================" << endl;
}