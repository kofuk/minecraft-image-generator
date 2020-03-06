#ifndef WORKER_HH
#define WORKER_HH

#include <mutex>
#include <utility>

#include "Region.hh"

using namespace std;

struct RegionContainer {
    Anvil::Region *region;
    int rx;
    int rz;

    RegionContainer (Anvil::Region *region, int rx, int rz);
    ~RegionContainer ();
};

struct QueuedItem {
    shared_ptr<RegionContainer> region;
    int off_x;
    int off_z;

    QueuedItem (shared_ptr<RegionContainer> region, int off_x, int off_z);

    string debug_string ();
};

extern string option_out_dir;
extern bool option_verbose;
extern int option_jobs;
extern bool option_nether;
extern bool option_generate_progress;
extern bool option_generate_range;
extern string option_journal_dir;

QueuedItem *fetch_item ();
void queue_item (QueuedItem *item);
void start_worker ();
void wait_for_worker ();

#endif
