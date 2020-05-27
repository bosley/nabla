#ifndef NABLA_LIB_MANIFEST
#define NABLA_LIB_MANIFEST

#include <string>
#include <map>
#include <libnabla/projectfs.hpp>
#include <libnabla/json.hpp>

namespace NABLA
{
    class LibManifest
    {
    public:
        LibManifest();
        ~LibManifest();

        bool load_manifest(std::string location);

        NABLA::ProjectFS get_library(std::string bundle, std::string lib, bool & found);

    private:

        // Maps Library bundle to individual library objects i.e   stdlib -> io  -> io_project_fs
        //                                                               \-> net -> net_project_fs
        std::map< std::string, std::map<std::string, NABLA::ProjectFS> > libraries;
    };
}

#endif