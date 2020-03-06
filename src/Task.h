#pragma once

#include <memory>
#include <iostream>
#include <boost/program_options.hpp>
#include "macros.h"



namespace pl {
    namespace po = boost::program_options;
    using options_description = boost::program_options::options_description;
    using variables_map = boost::program_options::variables_map;


    class Task  {
        DECLARE_NO_COPY_CLASS(Task);
    public:
        explicit Task();
        Task(const Task&&) =delete;
        ~Task();
        void run(int ac, char *av[]);
        void stop();
    private:
        void plugin_startup();
        void plugin_initialize(const variables_map& options);
        void rb_worker();//command from Task and next handler
        void rpc_listen() const;
        void rb_send(const std::string &data) const;
    private:
        std::unique_ptr<class TaskImpl> pImpl;
        std::unique_ptr<class ConsoleMenu> menu;


    };

/**
 * @brief console menu
 *
 * \details
 * parse arg command line
     default show menu
     set default value for rb_connection
 */
    class ConsoleMenu{
    public:
        variables_map parse_options(int ac, char *av[]);
    private:
        void set_program_options(options_description &cfg);
        friend class Task;
    };

}

