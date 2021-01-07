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

#pragma once
#include <string>

namespace LibSoprano
{
    class Color
    {
    public:
        Color(unsigned int foreground);

        static const Color BLACK;
        static const Color DARK_BLUE;
        static const Color DARK_GREEN;
        static const Color DARK_AQUA;
        static const Color DARK_RED;
        static const Color DARK_PURPLE;
        static const Color GOLD;
        static const Color GRAY;
        static const Color DARK_GRAY;
        static const Color BLUE;
        static const Color GREEN;
        static const Color AQUA;
        static const Color RED;
        static const Color LIGHT_PURPLE;
        static const Color YELLOW;
        static const Color WHITE;

        unsigned int foreground() const { return m_foreground; }
        unsigned int background() const { return m_background; }
        const char* ansi_color() const { return m_ansi_color; }
        const std::string& name() const { return m_name; }
        static const Color* from_name(const char*);

        static const char* m_ansi_reset;
        static const char* m_ansi_escape;
        static const char* m_ansi_escape_escaped;

    private:
        Color(unsigned int foreground, unsigned int background, const char* name, const char* ansi_color)
            : m_foreground(foreground), m_background(background), m_name(name), m_ansi_color(ansi_color) {}
        unsigned int m_foreground = 0;
        unsigned int m_background = 0;
        std::string m_name;
        const char* m_ansi_color = nullptr;
    };
}
