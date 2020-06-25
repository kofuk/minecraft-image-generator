#ifndef SERVER_HH
#define SERVER_HH

#include <string>

#include "request.hh"
#include "writer.hh"

using namespace std;

namespace pixel_terrain::commands::server {
    extern string overworld_dir;
    extern string nether_dir;
    extern string end_dir;

    void handle_request(request *req, writer *w);
    int main(int argc, char **argv);
} // namespace pixel_terrain::commands::server

#endif
