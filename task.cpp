#include "task.h"
#include <amqpcpp.h>
#include "SimplePocoHandler.h"
#include "thread_safe.hpp"
#include "rabbitmq_worker.hpp"
#include  <mutex>

#include <thread>
#include <algorithm>
#include <regex>
#include <iostream>
#include <exception>
#include <vector>
#include <functional>

namespace pl {
using std::cout;
using std::function;
using std::thread;
using std::mutex;
using std::shared_ptr;
using std::exception;
using std::vector;
using RB_client = function<void(string &data)>;

void rb_chanel_data(string data) {
    SimplePocoHandler handler("localhost", 5672);
    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
    AMQP::Channel channel(&connection);

    channel.onReady([&]() {
        if (handler.connected()) {
            channel.publish("", "hello", data);
            handler.quit();
        } else {
            printf("Handler not connected");
        }
    });
    handler.loop();
};


class task_impl {
    thread *th, *th_data;
    RB_client rb_res = rb_chanel_data;
    shared_ptr<thread_safe::threadsafe_queue<string>> q_from_rabbit;

    friend class task;

public:
    void rpc_server();

    void rpc_listen();

    void rb_chanel(string data);

    void send();

private:
    string queue_host = "localhost";
    string queue_name = "hello";
    string queue_rpc = "rpc_queue";
    uint32_t queue_port = 5672;
    string queue_login = "guest";
    string queue_passwd = "guest";
    mutex m_mutex;
    vector<std::exception_ptr> m_exceptions;


};


//    void task_impl::advanced() {
//            if(th != nullptr){
//                if(th->joinable()) {
//                    th->join();
//                    delete (th);
//                }
//
//            }
//
//            wrapper["data"] = resource_user;
//        th = new std::thread([&](fc::mutable_variant_object data) {rb_chanel(data);},wrapper);
//
//    }

void task_impl::rpc_listen() {

    try {
        string temp;
        if (q_from_rabbit->try_pop(temp)) {
            std::cout << "RECIEVED COMMAND" << temp;
            auto block = temp;

        } else {
            cout << "Command not found" << "\n";
        }
    }
    catch (...) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_exceptions.push_back(std::current_exception());

    }
}

void task_impl::rpc_server() {
    q_from_rabbit = std::make_shared<thread_safe::threadsafe_queue<string>>();

    /// 1-st thread starts
    thread t_rabbit([&](shared_ptr<thread_safe::threadsafe_queue<string>> queue_rabbit) {

        rabbitmq_worker rabbitmq_input(queue_rabbit, queue_host, queue_port, queue_login, queue_passwd, "/",
                                       queue_rpc);
        rabbitmq_input.run();
    }, q_from_rabbit);

    if (t_rabbit.joinable())
        t_rabbit.detach();

    cout << "Rabbit runs away!" << "\n";
}


void task_impl::rb_chanel(string data) {

    try {
        SimplePocoHandler handler(queue_host, queue_port);
        AMQP::Connection connection(&handler, AMQP::Login(queue_login, queue_passwd), "/");
        AMQP::Channel channel(&connection);

        channel.onReady([&]() {
            if (handler.connected()) {
                channel.publish("", "hello", data);
                handler.quit();
            } else {
                printf("Handler not connected");
            }
        });
        handler.loop();
    }
    catch (...) {
        m_exceptions.clear();
        std::lock_guard<std::mutex> lock(m_mutex);
        m_exceptions.push_back(std::current_exception());
        for (auto &ex: m_exceptions) {
            try {
                if (ex != nullptr)
                    std::rethrow_exception(ex);
            }
            catch (const std::exception &ex) {
                cout << ex.what() << std::endl;
            }
        }
    }
}


void task_impl::send() {
    if (th_data != nullptr) {
        if (th_data->joinable()) {
            th_data->join();
            delete (th_data);
        }
    }

    string wrapper = "test";
    th_data = new thread([&](string data) { rb_chanel(data); }, wrapper);
    cout << " Send to rabbit::" << wrapper << "\n";
//            th_data = new std::thread([&](fc::mutable_variant_object data) { rb_res(data);},wrapper);
}




    task::task() : my(new task_impl()) {}

    task::~task() {}

    void task::send() { my->send(); }

    void task::rpc_server() { my->rpc_server(); }

    void task::rpc_listen() { my->rpc_listen(); }

    void task::plugin_startup() {
        my->rpc_server();
        my->rpc_listen();
        my->th = nullptr;
        my->th_data = nullptr;
    }
}

int main() {

    auto rt = std::make_shared<pl::task>();
    rt->plugin_startup();

    while (1) {
        rt->send();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

};
