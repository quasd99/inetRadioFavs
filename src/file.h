#ifndef FILE_H
#define FILE_H

#include <string>

namespace gats
{
 
class File
{
    public:
    File();
    
    static bool is_regular_file(const std::string &f);

    private:
};

} // namespace gats
 
#endif // FILE_H
