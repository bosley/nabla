#ifndef DEL_ERRORS_HPP
#define DEL_ERRORS_HPP

#include <string>
#include <stdint.h>
#include "Tracker.hpp"

namespace DEL
{
    //! \class Errors
    //! \brief This is the primary error handler for the compiler. It has a handful of specific error messages
    //!        that we can tell use to trigger fatals. I would like to extend this for different output levels.
    class Errors
    {
    public:

        //! \brief Create the error object
        //! \param tracker The tracker object
        Errors(Tracker & tracker);

        //! \brief Deconstruct the error object
        ~Errors();

        //! \brief Report that something has already been declared
        //! \param id The thing that has already been declared
        //! \post This method call triggers exit failure
        void report_previously_declared(std::string id);

        //! \brief Report an unknown identifier
        //! \param id The id that was discovered and unknoen
        //! \param is_fatal Triggers exit if true
        void report_unknown_id(std::string id, bool is_fatal=true);

        //! \brief Report an unallowed type
        //! \param is The id that was being set to something incorrect
        //! \param is_fatal Triggers exit if true
        void report_unallowed_type(std::string id, bool is_fatal=true);

        //! \brief Report that the system has attempted to allocate a stack greater than allowed by VSys
        //! \param symbol The last symbol allocated that triggered the issue
        //! \param size The size of the object allocated last
        //! \param max_memory The maximum amount of memory the current system allows
        void report_out_of_memory(std::string symbol, uint64_t size, int max_memory);

        //! \brief Report unable to open a file for a result output
        //! \param name_used The name of the file
        //! \param is_fatal Triggers exit if true
        void report_unable_to_open_result_out(std::string name_used, bool is_fatal=true);

        //! \brief Report a custom error
        //! \param from Where the error originates
        //! \param error The error to output
        //! \param is_fatal Triggers exit if true
        //! \note This is mainly used for a developer error, really only things that should't happen
        //!       should utilize this
        void report_custom(std::string from, std::string error, bool is_fatal=true);

        //! \brief Report that a function doesn't have a matching return
        //! \param function The function name / details to output
        //! \post This method is a default fatal that will trigger exit
        void report_no_return(std::string function);

    private:
        Tracker &tracker;

        std::string get_error_start(bool is_fatal);
        std::string get_line_no();
    };
}
#endif