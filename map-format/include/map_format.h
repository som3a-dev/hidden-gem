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

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{

} mf_tilemap_t;


#ifdef __cplusplus
}
#endif
