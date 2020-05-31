#ifndef SERVER_HH
#define SERVER_HH

#include <string>

#include "request.hh"
#include "writer.hh"

using namespace std;

namespace mcmap::server {
    extern string overworld_dir;
    extern string nether_dir;
    extern string end_dir;

    void print_protocol_detail();

    void handle_request(request *req, writer *w);
    void launch_server(bool daemon_mode);
} // namespace mcmap::server

#endif