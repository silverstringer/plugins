#include "Task.h"
#include "SimplePocoHandler.h"
#include "thread_safe.hpp"
#include "rabbitmq_worker.hpp"
#include "LoggerCSV.hpp"
#include "Container.h"

#include <amqpcpp.h>
#include <mutex>
#include <thread>
#include <algorithm>
#include <regex>
#include <exception>
#include <vector>
#include <functional>

namespace plugin {
    using std::cout;
    using std::function;
    using std::thread;
    using std::mutex;
    using std::shared_ptr;
    using std::exception;
    using std::exception_ptr;
    using std::vector;
    using threadsafe_queue = thread_safe::threadsafe_queue<string>;
    using namespace plugin::logger;
    using namespace plugin::sota;

    class TaskImpl {
//        DECLARE_CONSTR_DESTR_DEFAULT_CLASS(TaskImpl);
        DECLARE_NO_COPY_CLASS(TaskImpl);
        thread *th, *th_data;
        shared_ptr<threadsafe_queue> q_from_rabbit;
        friend class Task;
    public:
        TaskImpl();
        ~TaskImpl() = default;
        void send(const string &data);
        string m_data;
    private:
        void rb_worker();
        void rpc_listen() const;
        void rb_publish(const string data);
        void initLogger();

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
        mutable std::unique_ptr<CSVWriter>logger;
        variables_map m_options;
        bool m_quit;


    };

    TaskImpl::TaskImpl():logger(std::make_unique<CSVWriter>()) {
             initLogger();
    }


    void TaskImpl::initLogger() {
        string current_filename = "logger.csv";
        string file_path ="log/"+ current_filename;
        if(bfs::exists(file_path))
            bfs::remove(file_path);
        logger->settings(true,"log", current_filename);
        vector<string> heading{ "event","data" };
        logger->addDatainRow(heading.begin(), heading.end());
    }

    void TaskImpl::rpc_listen() const {

        try {
            string temp;
            if (q_from_rabbit->try_pop(temp)) {
                std::cout << "Recieved Command" << temp;
                std::vector<std::string>m1{temp};
                logger->addDatainRow(m1.begin(),m1.end());
            } else {
                cout << "Command not found" << "\n";
            }
        }
        catch (...) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_exceptions.push_back(std::current_exception());

        }
    }

    void TaskImpl::rb_worker() {
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


    void TaskImpl::rb_publish(const string data) {

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
                    std::vector<std::string>m1{"exception",ex.what()};
                    logger->addDatainRow(m1.begin(),m1.end());
                }
            }
        }
    }


    void TaskImpl::send(const string &data) {
        if (th_data != nullptr) {
            if (th_data->joinable()) {
                th_data->join();
                delete (th_data);
            }
        }
        m_data = std::move(data);
        th_data = new thread([&](string m_data) { rb_publish(m_data); }, m_data);
        cout << " Send to rabbit::" << m_data << "\n";

        std::vector<std::string>m1{"send data", m_data};
        logger->addDatainRow(m1.begin(),m1.end());
       //            th_data = new std::thread([&](fc::mutable_variant_object data) { rb_res(data);},wrapper);
    }


    void ConsoleMenu::set_program_options(options_description &cfg) {
        cfg.add_options()
                ("help,h", "produce help message")
                ("queue-name", bpo::value<string>()->default_value("hello"), "Name for queue")
                ("queue-port", bpo::value<uint32_t>()->default_value(5672), "Port for queue.")
                ("queue-host", bpo::value<string>()->default_value("localhost"), "Host for queue")
                ("login", bpo::value<string>()->default_value("guest"), "Login for cleints")
                ("passwd", bpo::value<string>()->default_value("guest"), "Passwd for clients")
                ("data", bpo::value<string>()->default_value("test"), "Data for rb_queue");

    }

    variables_map ConsoleMenu::parse_options(int ac, char *av[]) {
        options_description desc("Allowed options");
        set_program_options(desc);
        variables_map vm;

        bpo::store(bpo::parse_command_line(ac, av, desc), vm);
        bpo::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;

        }
        return vm;

    };

    Task::Task() : pImpl(std::make_unique<TaskImpl>()),
                   menu(std::make_unique<ConsoleMenu>()),
                   sota (std::make_unique<SimpleSota>(49))
                   {
                       sota->setValue();
                       sota->removeRangeValue();
                       sota->synchronization();

                   }

    Task::~Task() {}

    void Task::rb_send(const string &data) const { pImpl->send(data); }

    void Task::rb_worker() { pImpl->rb_worker(); }

    void Task::rpc_listen() const { pImpl->rpc_listen(); }

    void Task::plugin_startup() {
        pImpl->rb_worker();
        pImpl->m_quit = false;
        pImpl->m_data =sota->toString();
        pImpl->th = nullptr;
        pImpl->th_data = nullptr;
    }

    void Task::run(int ac, char *av[]) {
        auto options = menu->parse_options(ac, av);
        plugin_initialize(options);
        plugin_startup();

        try {
            while (!pImpl->m_quit) {
                pImpl->rpc_listen();
                rb_send(pImpl->m_data);
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }

        }
        catch (exception &ex) {
            std::cerr << ex.what();
        }

    }

/**
 * @brief stop server: flag quit is true
 *
 */
    void Task::stop() {
        pImpl->m_quit = true;
    }


    void Task::plugin_initialize(const variables_map &options) {
        pImpl->m_options = &options;
        pImpl->rb_param.queue_name = options.at("queue-name").as<string>();
        pImpl->rb_param.queue_port = options.at("queue-port").as<uint32_t>();
        pImpl->rb_param.queue_host = options.at("queue-host").as<string>();
        pImpl->rb_param.queue_passwd = options.at("passwd").as<string>();
        pImpl->rb_param.queue_login = options.at("login").as<string>();
        pImpl->m_data = options.at("data").as<string>();

    }

}

