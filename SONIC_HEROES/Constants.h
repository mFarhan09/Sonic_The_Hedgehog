

#pragma once

namespace GameConstants {
    constexpr int SCREEN_X = 1200;
    constexpr int SCREEN_Y = 900;
    constexpr int CELL_SIZE = 64;
    constexpr int ROWS = 14;
    constexpr int COLS[] = {200,250,300};
    const int MAX_SHOTS = 8;
    // No fixed "max" counts here—levels configure their own sizes.
    constexpr int MAX_TOP_SCORES = 10;  // for Leaderboard
}
