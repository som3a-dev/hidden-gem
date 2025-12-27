#ifndef _TILEMAP_H
#define _TILEMAP_H

#define INVALID_TILE -1
#define EMPTY_TILE 0

struct Tilemap
{
    void create(int w, int h);
    void clear();
    void set_tile(int x, int y, int id);
    int get_tile(int x, int y) const;

    int get_width() const
    {
        return width;
    }
    int get_height() const 
    {
        return height;
    }

    ~Tilemap();
private:
    int* tiles = nullptr;
    int width = 0;
    int height = 0;
};

#endif