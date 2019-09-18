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


    class task_impl {
        thread *th, *th_data;
        shared_ptr<thread_safe::threadsafe_queue<string>> q_from_rabbit;

        friend class task;

    public:
        void rpc_server();

        void rpc_listen() const;

        void rb_chanel(string data);

        void send(const string &data);

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
        mutable vector<std::exception_ptr> m_exceptions;
        boost::program_options::variables_map _options;


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

    void task_impl::rpc_server() {
        q_from_rabbit = std::make_shared<thread_safe::threadsafe_queue<string>>();

        /// 1-st thread starts
        thread t_rabbit([&](shared_ptr<thread_safe::threadsafe_queue<string>> queue_rabbit) {

            rabbitmq_worker rabbitmq_input(queue_rabbit, rb_param.queue_host, rb_param.queue_port, rb_param.queue_login,
                                           rb_param.queue_passwd, "/",
                                           rb_param.queue_rpc);
            rabbitmq_input.run();
        }, q_from_rabbit);

        if (t_rabbit.joinable())
            t_rabbit.detach();

        cout << "Rabbit runs away!" << "\n";
    }


    void task_impl::rb_chanel(string data) {

        try {
            SimplePocoHandler handler(rb_param.queue_host, rb_param.queue_port);
            AMQP::Connection connection(&handler, AMQP::Login(rb_param.queue_login, rb_param.queue_passwd), "/");
            AMQP::Channel channel(&connection);

            channel.onReady([&]() {
                if (handler.connected()) {
                    channel.publish("", rb_param.queue_name, data);
                    handler.quit();
                } else {
                    cout << "Handler not connected";
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
        cout << " Send to rabbit::" << data << "\n";
        th_data = new thread([&](string data) { rb_chanel(data); }, data);
        cout << " Send to rabbit::" << data << "\n";
//            th_data = new std::thread([&](fc::mutable_variant_object data) { rb_res(data);},wrapper);
    }


    task::task() : my(new task_impl()) {}

    task::~task() {}

    void task::send_rb(const string &data) const { my->send(data); }

    void task::rpc_server() { my->rpc_server(); }

    void task::rpc_listen() const { my->rpc_listen(); }

    void task::plugin_startup() {
        my->rpc_server();
        my->rpc_listen();
        my->th = nullptr;
        my->th_data = nullptr;
    }

    void task::set_program_options(po::options_description &, po::options_description &cfg) {
        cfg.add_options()
                ("queue-name", po::value<string>()->default_value("hello"), "Name for queue")
                ("queue-port", po::value<uint32_t>()->default_value(5672), "Port for queue.")
                ("queue-host", po::value<string>()->default_value("localhost"), "Host for queue")
                ("login", po::value<string>()->default_value("guest"), "Login for cleints")
                ("passwd", po::value<string>()->default_value("guest"), "Passwd for clients");

    }

    void task::plugin_initialize(const boost::program_options::variables_map &options) {
        my->_options = &options;
        my->rb_param.queue_name = options.at("queue-name").as<string>();
        my->rb_param.queue_port = options.at("queue-port").as<uint32_t>();
        my->rb_param.queue_host = options.at("queue-host").as<string>();
        my->rb_param.queue_passwd = options.at("passwd").as<string>();
        my->rb_param.queue_login = options.at("login").as<string>();

    }


    void set_program_options(po::options_description &cfg) {
        cfg.add_options()
                ("help,h", "produce help message")
                ("queue-name", po::value<string>()->default_value("hello"), "Name for queue")
                ("queue-port", po::value<uint32_t>()->default_value(5672), "Port for queue.")
                ("queue-host", po::value<string>()->default_value("localhost"), "Host for queue")
                ("login", po::value<string>()->default_value("guest"), "Login for cleints")
                ("passwd", po::value<string>()->default_value("guest"), "Passwd for clients");

    }

    void plugin_initialize(const boost::program_options::variables_map &options) {
        boost::program_options::variables_map _options = &options;
        string queue_name = options.at("queue-name").as<string>();
        auto queue_port = options.at("queue-port").as<uint32_t>();
        auto queue_host = options.at("queue-host").as<string>();
        auto queue_passwd = options.at("passwd").as<string>();
        auto queue_login = options.at("login").as<string>();

    }

    void console_menu(int ac, char *av[]) {

        boost::program_options::options_description desc("Allowed options");
        pl::set_program_options(desc);
        pl::po::variables_map vm;
        pl::po::parsed_options parsed = pl::po::command_line_parser(ac, av).options(desc).allow_unregistered().run();
        pl::po::store(parsed, vm);
        pl::po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;

        }

    };


}

int main(int ac, char *av[]) {

    pl::console_menu(ac, av);
    auto rt = std::make_shared<pl::task>();
    rt->plugin_startup();

    while (1) {
        rt->send_rb("test");
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

};
