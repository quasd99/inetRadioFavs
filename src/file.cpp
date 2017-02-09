#include "file.h"

#include <iostream>

#include <boost/filesystem.hpp>

using namespace gats;
 
File::File()
{
}

bool File::is_regular_file(const std::string& f)
{
    boost::filesystem::path p(f);
    
    if ( boost::filesystem::is_regular(p) )
    {
        return true;
    }
   
    std::cerr << "  ERR:" << __PRETTY_FUNCTION__
            << ":cannot access file:"
            << f
            << std::endl;

    return false;
}
