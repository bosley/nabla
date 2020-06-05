#ifndef DEL_ERRORS_HPP
#define DEL_ERRORS_HPP

#include <string>

#include "Tracker.hpp"

namespace DEL
{
    class Errors
    {
    public:
        Errors(Tracker & tracker);
        ~Errors();

        void report_previously_declared(std::string id);

        void report_unknown_id(std::string id, bool is_fatal=true);

        void report_unallowed_type(std::string id, bool is_fatal=true);

        void report_custom(std::string from, std::string error, bool is_fatal=true);

    private:
        Tracker &tracker;

        std::string get_error_start(bool is_fatal);
    };
}
#endif