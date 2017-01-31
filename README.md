chunked-cpp
==

C++11 implementation of [chunked](https://github.com/unn4m3d/chunked)

Writer is not implemented

Usage
--

```c++
#include <fstream>
#include <chunked.hpp>
#include <iostream>

using chunked::reader;

std::ifstream str("/path/to/file", std::ifstream::binary);
reader<size_t, size_t> rd(str);

/*chunked::chunk_info<size_t, size_t>*/
auto cinfo = rd.open_chunk();

std::cout << cinfo.get_index() << " " << cinfo.get_size() << std::endl;

/*chunked::ichunk<size_t, size_t>*/
auto chunk = rd.current_chunk(c);

/*chunk.buffer is a vector<char>*/
std::cout << chunk.buffer.size() << std::endl;

```
