#include "async.h"
#include <string>
#include <thread>

int main() {
    std::thread th1([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        const std::string commands1 = "th1_cmd1\nth1_cmd2\nth1_cmd3\nth1_cmd4\nth1_cmd5\n";
        const std::string commands2 = "th1_cmd1\nth1_cmd2\n{\nth1_cmd3\nth1_cmd4\n}\n";
        const std::string commands3 = "{\nth1_cmd5\nth1_cmd6\n{\nth1_cmd7\nth1_cmd8\n}\nth1_cmd9\n}\n";
        const std::string commands4 = "{\nth1_cmd10\nth1_cmd11\n";

        const auto id = connect(3);
        receive(id, commands1);
        receive(id, commands2);
        receive(id, commands3);
        receive(id, commands4);
        disconnect(id);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    });

    std::thread th2([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        const std::string commands1 = "th2_cmd1\nth2_cmd2\nth2_cmd3\nth2_cmd4\nth2_cmd5\n";
        const std::string commands2 = "th2_cmd1\nth2_cmd2\n{\nth2_cmd3\nth2_cmd4\n}\n";
        const std::string commands3 = "{\nth2_cmd5\nth2_cmd6\n{\nth2_cmd7\nth2_cmd8\n}\nth2_cmd9\n}\n";
        const std::string commands4 = "{\nth2_cmd10\nth2_cmd11\n";

        const auto id = connect(3);
        receive(id, commands1);
        receive(id, commands2);
        receive(id, commands3);
        receive(id, commands4);
        disconnect(id);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    });

    std::thread th3([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        const std::string commands1 = "th3_cmd1\nth3_cmd2\nth3_cmd3\nth3_cmd4\nth3_cmd5\n";
        const std::string commands2 = "th3_cmd1\nth3_cmd2\n{\nth3_cmd3\nth3_cmd4\n}\n";
        const std::string commands3 = "{\nth3_cmd5\nth3_cmd6\n{\nth3_cmd7\nth3_cmd8\n}\nth3_cmd9\n}\n";
        const std::string commands4 = "{\nth3_cmd10\nth3_cmd11\n";

        const auto id = connect(3);
        receive(id, commands1);
        receive(id, commands2);
        receive(id, commands3);
        receive(id, commands4);
        disconnect(id);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    });

    th1.join();
    th2.join();
    th3.join();

    return 0;
}