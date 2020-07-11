#ifndef MIDIVEC_HPP
#define MIDIVEC_HPP
#include <vector>   // For std::vector<>
#include <cstring> // for std::strlen

typedef unsigned char byte;

/* First define a custom wrapper over std::vector<byte>
 * so we can quickly push_back multiple bytes with a single call.
 */
class MIDIvec: public std::vector<byte>
{
    public:
        // Methods for appending raw data into the vector:
        template<typename... Args>
            void AddBytes(byte data, Args...args)
            {
                push_back(data);
                AddBytes(args...);
            }
        template<typename... Args>
            void AddBytes(const char* s, Args...args)
            {
                insert(end(), s, s + std::strlen(s));
                AddBytes(args...);
            }
        void AddBytes() { }
};
#endif
