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
    using std::exception_ptr;
    using std::vector;
    using threadsafe_queue=thread_safe::threadsafe_queue<string>;
    class task_impl {
        thread *th, *th_data;
        shared_ptr<threadsafe_queue> q_from_rabbit;
        friend class task;
    public:
        void send(const string &data);
        string m_data;
    private:
        void rb_worker();
        void rpc_listen() const;
        void rb_publish(const string data);
    private:
        struct {
            string queue_host = "localhost";
            string queue_name = "hello";
            string queue_rpc = "rpc_queue";
            uint32_t queue_port = 5672;
            string queue_login = "guest";
            string queue_passwd = "guest";
        } rb_param;
        mutable mutex m_mutex;
        mutable vector<exception_ptr> m_exceptions;
        variables_map _options;
        bool quit;

    };

    void task_impl::rpc_listen() const {

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

    void task_impl::rb_worker() {
        q_from_rabbit = std::make_shared<threadsafe_queue>();

        /// 1-st thread starts
        thread t_rabbit([&](shared_ptr<threadsafe_queue> queue_rabbit) {

            rabbitmq_worker rabbitmq_input(queue_rabbit, rb_param.queue_host, rb_param.queue_port, rb_param.queue_login,
                                           rb_param.queue_passwd, "/",
                                           rb_param.queue_rpc);
            rabbitmq_input.run();
        }, q_from_rabbit);

        if (t_rabbit.joinable())
            t_rabbit.detach();

        cout << "Rabbit runs away!" << "\n";
    }


    void task_impl::rb_publish(const string data) {

        try {
            SimplePocoHandler handler(rb_param.queue_host, rb_param.queue_port);
            AMQP::Connection connection(&handler, AMQP::Login(rb_param.queue_login, rb_param.queue_passwd), "/");
            AMQP::Channel channel(&connection);

            channel.onReady([&]() {
                if (handler.connected()) {
                    channel.publish("", rb_param.queue_name, data);
                    handler.quit();
                } else {
                    std::cerr << "Handler not connected";
                }
            });
            handler.loop();
        }
        catch (...) {
            m_exceptions.clear();
            std::lock_guard<std::mutex> lock(m_mutex);
            m_exceptions.push_back(std::current_exception());
            for (const auto &ex: m_exceptions) {
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


    void task_impl::send(const string &data) {
        if (th_data != nullptr) {
            if (th_data->joinable()) {
                th_data->join();
                delete (th_data);
            }
        }
        m_data = data;
        th_data = new thread([&](string data) { rb_publish(m_data); }, m_data);
        cout << " Send to rabbit::" << m_data<< "\n";
//            th_data = new std::thread([&](fc::mutable_variant_object data) { rb_res(data);},wrapper);
    }



    void console_menu::set_program_options(options_description &cfg) {
        cfg.add_options()
                ("help,h", "produce help message")
                ("queue-name", po::value<string>()->default_value("hello"), "Name for queue")
                ("queue-port", po::value<uint32_t>()->default_value(5672), "Port for queue.")
                ("queue-host", po::value<string>()->default_value("localhost"), "Host for queue")
                ("login", po::value<string>()->default_value("guest"), "Login for cleints")
                ("passwd", po::value<string>()->default_value("guest"), "Passwd for clients")
                ("data", po::value<string>()->default_value("test"), "Data for rb_queue");

    }

    variables_map console_menu::parse_options(int ac, char *av[])  {
        options_description desc("Allowed options");
        set_program_options(desc);
        variables_map vm;

        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;

        }
        return vm;

    };

    task::task() : my(new task_impl()),m_menu(new console_menu()){}

    task::~task() {}

    void task::rb_send(const string &data) const { my->send(data); }

    void task::rb_worker() { my->rb_worker(); }

    void task::rpc_listen() const { my->rpc_listen(); }

    void task::plugin_startup() {
        my->rb_worker();
        my->quit = false;
        my->m_data = "test";
        my->th = nullptr;
        my->th_data = nullptr;
    }

    void task::run(int ac, char *av[]) {
        auto options= m_menu->parse_options(ac,av);
        plugin_initialize(options);
        plugin_startup();

        try {
            while (!my->quit){
                my->rpc_listen();
                rb_send("test");
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }

        }
        catch(exception &ex)
        {
            std::cerr<<ex.what();
        }

    }

/**
 * @brief stop server: flag quit is true
 *
 */
    void task::stop() {
        my->quit = true;
    }


    void task::plugin_initialize(const variables_map &options) {
        my->_options = &options;
        my->rb_param.queue_name = options.at("queue-name").as<string>();
        my->rb_param.queue_port = options.at("queue-port").as<uint32_t>();
        my->rb_param.queue_host = options.at("queue-host").as<string>();
        my->rb_param.queue_passwd = options.at("passwd").as<string>();
        my->rb_param.queue_login = options.at("login").as<string>();
        my->m_data = options.at("data").as<string>();

    }

}

int main(int ac, char *av[]) {
    auto my_plugin = std::make_shared<pl::task>();
    my_plugin->run(ac,av);


};
