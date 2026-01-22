/*
    This project is the library (name: map format) to use for parsing and saving file formats related to the game, map editor and any future adjacent projects.

    This file (map_format.h) is the main header for the library.

    More rules about the library's job, These MUST be followed:
        - This library defines the canonical on-disk representation
        of maps and tilesets. Callers must convert to and from their
        own internal representations.
        For example: I don't care how you define a tilemap, but you must give me and expect me to give you
        my representation of it, a mf_tilemap_t or equivalent (specific type names are not set in stone). 

        - This library is intentionally dumb about gameplay. Meaning it does not care about gameplay and shouldn't be modified to work for specific
        gameplay related needs. More on that later in the (NOTE)s.



    Some code specific details:
        - The API is abbreviated as mf or MF.
        - All memory ownership rules are defined per-function. there is no hard standard for now.

        - Notes about certain types:
            - mf_tileid_t is a signed 32 bit integer type that is used for tile ids
            a tile id of 0 signifies an empty tile,
            tile ids of negative numbers should not be used, as they are left for
            reserved tile ids used by the format.
            
            - mf_mapsz_t is a unsigned 32 bit integer type that is used for the dimensions
            and positions in a tilemap


    Below is a comprehensive list of the formats that are defined:

    *MAPS* (.hgm extension)
    Maps are defined with a propreitary format that is defined as, in order:
    - A Tilemap:
        - Two 4 byte signed integers defining the width and height of the tilemap
        - Tightly packed 4 byte signed integer tile ids that define the tiles
        in the tilemap
    - NOTE: Reserved tile ids, like an id that signifies an empty tile, are defined by this library.
    And are not up to the user's interpretation in this format. You can have your own tilemap format you convert to with your own way to
    decide what signifies an empty tile, But here, an empty tile has an id of MF_TILE_EMPTY (or equivalent)
    
    *TILESETS* (.json extension)
    Tilesets are defined as a json file that contains:
    - An array of objects:
        - Each object defines a tile's properties, like for example id and texture_id.
        What the properties are is left up to the users of the library (Like the game and editor) to decide upon.
    
    - NOTE: Regarding the tileset json format, we only validate the format of the
    specified json file (An array of objects, valid json) and return it as a string to the caller, we do not verify
    any specific properties existence although this can be explored in the future.
    - So parsing a tileset json file using this library means giving the file path, being informed of any errors in the format,
    and if there are none you get the file as a normal null terminated string.
*/

#ifndef _MAP_FORMAT_H
#define _MAP_FORMAT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

// This id signifies a new row in the map (everytime you see it in the file, the tile after it is the start of a new row)
#define MF_TILE_ROW -2

// This id signifies an invalid tile, this is only used as a return value for some API functions
// that return a mf_tileid_t
#define MF_TILE_INVALID -1

// This id signifies an empty tile
#define MF_TILE_EMPTY 0

typedef int32_t mf_tileid_t;
typedef uint32_t mf_mapsz_t;

typedef struct
{
    // Size in tiles
    mf_mapsz_t w; 
    mf_mapsz_t h; 

    // Tiles packed in 1d array
    mf_tileid_t* tiles;
} mf_tilemap_t;

/*
* Loads a tilemap from a map file
* the map's tiles array is allocated on the heap, caller should free() after use or call mf_tilemap_destroy
* Returns an empty mf_tilemap_t (zeroed out) if the file was invalid (invalid format/doesn't exist)
*/
mf_tilemap_t mf_load_tilemap(const char* filepath);

/*
* Saves a tilemap into a file
* Overwrites anything in the file if it exists
* returns true/false on success/failure
*/
bool mf_save_tilemap(const char* filepath, const mf_tilemap_t* map);

/*
* Creates an empty tilemap (filled with MF_TILE_EMPTY) of the desired size
* Returns a zeroed out mf_tilemap_t on failure
*/
mf_tilemap_t mf_tilemap_create(mf_mapsz_t w, mf_mapsz_t h);

/*
* Frees memory allocated in a tilemap returned by mf_create_tilemap
*/
void mf_tilemap_destroy(mf_tilemap_t* map);

/*
* Sets a tile in the tilemap to a specified value
*/
void mf_tilemap_set(mf_tilemap_t* map, mf_mapsz_t x, mf_mapsz_t y, mf_tileid_t tile);

/*
* Gets the tile in a tilemap at a specified position
* Returns a MF_TILE_INVALID on failure
*/
mf_tileid_t mf_tilemap_get(const mf_tilemap_t* map, mf_mapsz_t x, mf_mapsz_t y);

#ifdef __cplusplus
}
#endif

#endif