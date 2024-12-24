#include "httplib.h"

std::string g_str;
std::mutex g_mutex;

void on_hello(const httplib::Request& req, httplib::Response& res)
{
//    g_mutex.lock();

    std::lock_guard<std::mutex> lock(g_mutex);

    g_str = req.body;

    std::cout << "Hello From Client!" << std::endl;

    res.set_content("Hello From Server!", "text/plain");

//    g_mutex.unlock();
}

int main(int argc, char** argv)
{
    httplib::Server server;

    server.Post("/hello", on_hello);

    server.listen("localhost", 25565);

    return 0;
}