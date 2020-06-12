#ifndef REGION_HH
#define REGION_HH

#include <memory>
#include <string>

#include "Chunk.hh"
#include "file.hh"
#include "nbt.hh"

namespace pixel_terrain::anvil {
    class Region {
        unique_ptr<file<unsigned char>> data;
        size_t len;
        unique_ptr<file<uint64_t>> last_update;

        size_t header_offset(int chunk_x, int chunk_z);
        size_t chunk_location_off(int chunk_x, int chunk_z);
        size_t chunk_location_sectors(int chunk_x, int chunk_z);
        nbt::NBTFile *chunk_data(int chunk_x, int chunk_z);

    public:
        /* Construct new region object from given buffer of *.mca file content
         */
        Region(string file_name);
        Region(string file_name, string journal_dir);
        Chunk *get_chunk(int chunk_x, int chunk_z);
        Chunk *get_chunk_if_dirty(int chunk_x, int chunk_z);
    };
} // namespace pixel_terrain::anvil

#endif
