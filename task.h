#ifndef PLUGINS_TASK_H
#define PLUGINS_TASK_H
#pragma once

#include <memory>
#include <iostream>
#include <boost/program_options.hpp>
namespace pl {
    using namespace boost;
    namespace po = boost::program_options;
 class task  {
    public:
        explicit task();
        task(const task&) =delete;
        task(const task&&) =delete;
        task& operator =(const task&)=delete;
        virtual ~task();
        void plugin_startup(const po::variables_map &options);
        void send_rb(const std::string &data) const;
 private:
     void plugin_initialize(const po::variables_map& options);
     void rpc_server();//command from task and next handler
     void rpc_listen() const;
 private:
        std::unique_ptr<class task_impl> my;
    };
}

#endif //PLUGINS_TASK_H
