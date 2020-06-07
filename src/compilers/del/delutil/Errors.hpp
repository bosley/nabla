#ifndef DEL_ERRORS_HPP
#define DEL_ERRORS_HPP

#include <string>
#include <stdint.h>
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

        void report_out_of_memory(std::string symbol, uint64_t size, int max_memory);

        void report_unable_to_open_result_out(std::string name_used, bool is_fatal=true);

        void report_custom(std::string from, std::string error, bool is_fatal=true);

        void report_no_return(std::string function);

    private:
        Tracker &tracker;

        std::string get_error_start(bool is_fatal);
        std::string get_line_no();
    };
}
#endif