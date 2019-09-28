#ifndef PLUGINS_TASK_H
#define PLUGINS_TASK_H
#pragma once

#include <memory>
#include <iostream>
#include <boost/program_options.hpp>
namespace pl {
    namespace po = boost::program_options;
    using options_description = boost::program_options::options_description;
    using variables_map = boost::program_options::variables_map;


    class task  {
    public:
        explicit task();
        task(const task&) =delete;
        task(const task&&) =delete;
        task& operator =(const task&)=delete;
        ~task();
        void run(int ac, char *av[]);
        void stop();
    private:
        void plugin_startup();
        void plugin_initialize(const variables_map& options);
        void rb_worker();//command from task and next handler
        void rpc_listen() const;
        void rb_send(const std::string &data) const;
    private:
        std::unique_ptr<class task_impl> my;
        std::unique_ptr<class console_menu> m_menu;

    };


    class console_menu{
    public:
        variables_map parse_options(int ac, char *av[]);
    private:
        void set_program_options(options_description &cfg);
        friend class task;
    };

}

#endif //PLUGINS_TASK_H
