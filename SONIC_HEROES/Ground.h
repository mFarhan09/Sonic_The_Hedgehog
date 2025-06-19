#pragma once
#ifndef GROUND_H
#define GROUND_H

#include "Brick.h"

void placeGround(Tile* grid[20][30], int groundRows) {
    for (int i = 20 - groundRows; i < 20; ++i) {
        for (int j = 0; j < 30; ++j) {
            if (grid[i][j] != nullptr) {
                delete grid[i][j];
            }
            grid[i][j] = new Brick();
        }
    }
}

#endif



