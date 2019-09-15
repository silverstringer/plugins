#ifndef PLUGINS_TASK_H
#define PLUGINS_TASK_H
#pragma once

#include <memory>
#include <iostream>
namespace pl {
    class task {
    public:
        task();
        task(const task&)=delete;
        task(const task&&) =delete;
        task& operator =(const task&) =delete;
        virtual ~task();
        void rpc_server();//command from task and next handler
        void rpc_listen() const;
        void plugin_startup();
        void send(const std::string &data) const;
    private:
        std::unique_ptr<class task_impl> my;
    };
}

#endif //PLUGINS_TASK_H
