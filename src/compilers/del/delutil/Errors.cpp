#include "Errors.hpp"
#include "del_driver.hpp"

namespace DEL
{
    Errors::Errors(Tracker & tracker) : tracker(tracker)
    {

    }

    Errors::~Errors()
    {

    }

    void Errors::report_previously_declared(std::string id)
    {
        std::cerr << get_error_start(true) << "Symbol \"" << id << "\" already defined" << std::endl;

        exit(EXIT_FAILURE);
    }

    void Errors::report_unknown_id(std::string id, bool is_fatal)
    {
        std::cerr << get_error_start(is_fatal) << "Unknown ID \"" << id << "\"" << std::endl;

        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }
    }


    void Errors::report_unallowed_type(std::string id, bool is_fatal)
    {
        std::cerr << get_error_start(is_fatal) << "Type of \"" 
                  << id 
                  << "\" Forbids current operation"
                  << std::endl;
          
        if(is_fatal)
        {
            exit(EXIT_FAILURE);
        }        
    }

    std::string Errors::get_error_start(bool is_fatal)
    {
        std::string level = (is_fatal) ? "<FATAL>" : "<WARNING>";

        //std::string es = "[Error] " + level + " (line " + std::to_string(tracker.lines_count) + ") : ";

        // I've disabled lines here until we get something in place for tracking all lines and columns with the tracker
        
        std::string es = "[Error] " + level + " : ";
        return es;
    }
}