#ifndef PLUGINS_TASK_H
#define PLUGINS_TASK_H
#pragma once

#include <memory>

    class task {
    public:
        task();
        virtual ~task();
        void rpc_server();//command from task and next handler
        void rpc_listen();
        void plugin_startup();
        void send();
    private:
        std::unique_ptr<class task_impl> my;
    };

#endif //PLUGINS_TASK_H
