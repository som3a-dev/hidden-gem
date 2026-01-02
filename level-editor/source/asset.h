/*
* This module (asset.c & asset.h) acts as a singleton asset manager for the editor
* With functions for loading and retrieving each asset type
* Assets are identified with their absolute path
*/

#ifndef _ASSET_H
#define _ASSET_H

#include <raylib.h>

void asset_load_texture(const char* path);

// Returns NULL if the texture doesn't exist
Texture* asset_get_texture(const char* path);

void asset_delete_assets();

#endif