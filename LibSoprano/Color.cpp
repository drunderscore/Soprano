// Copyright James Puleo 2021
// Copyright LibSoprano 2021

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Color.h"
#include <fmt/format.h>

namespace LibSoprano
{
    const char* Color::m_ansi_escape_escaped = "\\u001b";
    const char* Color::m_ansi_escape = "\u001b";
    const char* Color::m_ansi_reset = "[0m";

    // TOOD: Should the alpha be set to 0xFF?
    const Color Color::BLACK =          {0,         0,          "black",        "[30m"};
    const Color Color::DARK_BLUE =      {0xAA,      0x2A,       "dark_blue",    "[34m"};
    const Color Color::DARK_GREEN =     {0xAA00,    0x2A00,     "dark_green",   "[32m"};
    const Color Color::DARK_AQUA =      {0xAAAA,    0x2A2A,     "dark_aqua",    "[36m"};
    const Color Color::DARK_RED =       {0xAA0000,  0x2A0000,   "dark_red",     "[31m"};
    const Color Color::DARK_PURPLE =    {0xAA00AA,  0x2A002A,   "dark_purple",  "[35m"};
    const Color Color::GOLD =           {0xFFAA00,  0x2A2A00,   "gold",         "[33m"};
    const Color Color::GRAY =           {0xAAAAAA,  0x2A2A2A,   "gray",         m_ansi_reset}; // TODO: Find gray ANSI
    const Color Color::DARK_GRAY =      {0x555555,  0x151515,   "dark_gray",    m_ansi_reset}; // TODO: Find dark gray ANSI
    const Color Color::BLUE =           {0x5555FF,  0x15153F,   "blue",         "[34;1m"};
    const Color Color::GREEN =          {0x55FF55,  0x153F15,   "green",        "[32;1m"};
    const Color Color::AQUA =           {0x55FFFF,  0x153F3F,   "aqua",         "[36;1m"};
    const Color Color::RED =            {0xFF5555,  0x3F1515,   "red",          "[31;1m"};
    const Color Color::LIGHT_PURPLE =   {0xFF55FF,  0x3F153F,   "light_purple", "[35;1m"};
    const Color Color::YELLOW =         {0xFFFF55,  0x3F3F15,   "yellow",       "[33;1m"};
    const Color Color::WHITE =          {0xFFFFFF,  0x3F3F3F,   "white",        "[37;1m"};

    Color::Color(unsigned int foreground) : m_foreground(foreground), m_name(fmt::format("#{:x}", foreground)) {}

    const Color* Color::from_name(const char* col_name)
    {
#define COLOR_BY_NAME(col) \
    if(col.name() == col_name) \
        return &col;

        COLOR_BY_NAME(BLACK);
        COLOR_BY_NAME(DARK_BLUE);
        COLOR_BY_NAME(DARK_GREEN);
        COLOR_BY_NAME(DARK_AQUA);
        COLOR_BY_NAME(DARK_RED);
        COLOR_BY_NAME(DARK_PURPLE);
        COLOR_BY_NAME(GOLD);
        COLOR_BY_NAME(GRAY);
        COLOR_BY_NAME(DARK_GRAY);
        COLOR_BY_NAME(BLUE);
        COLOR_BY_NAME(GREEN);
        COLOR_BY_NAME(AQUA);
        COLOR_BY_NAME(RED);
        COLOR_BY_NAME(LIGHT_PURPLE);
        COLOR_BY_NAME(YELLOW);
        COLOR_BY_NAME(WHITE);
        return nullptr;

#undef COLOR_BY_NAME
    }
}
