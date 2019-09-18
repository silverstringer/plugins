#ifndef PLUGINS_TASK_H
#define PLUGINS_TASK_H
#pragma once

#include <memory>
#include <iostream>
#include <boost/program_options.hpp>
namespace pl {
    using namespace boost;
    namespace po = boost::program_options;
 class task {
    public:
        task();
        task(const task&) =delete;
        task(const task&&) =delete;
        task& operator =(const task&)=delete;
        virtual ~task();
        void set_program_options(po::options_description&, po::options_description& cfg);
        void plugin_initialize(const po::variables_map& options);
        void rpc_server();//command from task and next handler
        void rpc_listen() const;
        void plugin_startup();
        void send_rb(const std::string &data) const;
 private:
        std::unique_ptr<class task_impl> my;
    };
}

#endif //PLUGINS_TASK_H
