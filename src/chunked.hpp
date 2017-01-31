#pragma once
#include <iostream>
#include <ostream>
#include <algorithm>
#include <vector>

namespace chunked 
{
    
    bool little_endian()
    {
        union{
            char c[4];
            uint32_t l;
        } u;
        u.l = 0x01020304;
        return u.c[0] == 0x04;
    }
    
    template<typename T>
    T to_le(T input)
    {
        if(little_endian()) return input;
        char buf[sizeof(T)];
        *((T*)buf) = input;
        std::reverse(&buf[0], &buf[sizeof(T)-1]);
        T output = *((T*)buf);
        return output;
    }
    
    template<typename TS, typename TV, typename TE>
    constexpr bool between(TS start, TV val, TE end )
    {
        return (val >= start && val <= end);
    }
    
    template<typename TIdx, typename TSz>
    class chunk_info
    {
    public:
        TIdx get_index()
        {
            return to_le(index);
        }
        TSz get_size()
        {
            return to_le(size);
        }
        
        static chunk_info<TIdx,TSz> read_from(std::istream *str)
        {
            const size_t size = sizeof(chunk_info<TIdx,TSz>);
            auto buf = new char[size];
            str->read(buf,size);
            if(str->rdstate())
                throw "Cannot read chunk";
            return *((chunk_info<TIdx,TSz>*)buf);
        }
        void write_to(std::ostream *str)
        {
            const size_t size = sizeof(chunk_info<TIdx,TSz>);
            str->write((char*)this, size);
        }
        
        chunk_info(TIdx idx, TSz size)
        {
            this->index = idx;
            this->size = size;
        }
        
        chunk_info() : index(0), size(0){}
    private:
        TIdx index; 
        TSz size;
    };
    
    template<typename TIdx, typename TSz>
    struct ichunk
    {
        std::vector<char> buffer;
        chunk_info<TIdx, TSz> info;
        size_t offset;
        
        ichunk(std::vector<char> &b, chunk_info<TIdx, TSz> info, size_t offset)
        {
            this->buffer = b;
            this->info = info;
            this->offset = offset;
        }
    };
    
    template<typename TIdx, typename TSz>
    class reader
    {
        using chunk_info_type = chunk_info<TIdx, TSz>;
        using chunk_type = ichunk<TIdx,TSz>;
    public:
        chunk_info_type open_chunk()
        {
            auto cinfo = chunk_info_type::read_from(stream);
            auto pos = stream->tellg();
            start_offsets.push_back(pos);
            end_offsets.push_back((size_t)pos + cinfo.get_size());
            return cinfo;
        }
        
        
        void close_chunk()
        {
            auto pos = stream->tellg();
            auto ep = end_offsets.back();
            end_offsets.pop_back();
            auto sp = start_offsets.back();
            start_offsets.pop_back();
            if(!between(sp,pos,ep))
            {
                start_offsets.push_back(sp);
                end_offsets.push_back(ep);
                throw "Current position isn't in chunk bounds";
            }
            stream->seekg(ep);
        }
        
        chunk_type current_chunk(chunk_info_type &info)
        {
            auto sp = start_offsets.back();
            auto ep = end_offsets.back();
            std::vector<char> buffer(ep - sp);
            stream->read(&buffer[0], ep - sp);
            return chunk_type(buffer, info, sp);
        }
        
        chunk_info<TIdx, TSz> find(TIdx idx)
        {
               for(chunk_info<TIdx, TSz> c; c = open_chunk();)
               {
                   if(c.get_index == idx) return c;
               }
               throw "Cannot find chunk!";
        }
        
        reader(std::istream *str)
        {
            this->stream = str;
        }
    private:
        std::istream *stream;
        std::vector<size_t> start_offsets;
        std::vector<size_t> end_offsets;
    };
}