#include "Task.h"

int main(int ac, char *av[]) {

    auto my_task = std::make_shared<plugin::Task>();
    my_task->run(ac, av);

};
