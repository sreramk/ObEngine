//Author : Sygmei
//Key : 976938ef7d46c286a2027d73f3a99467bcfa8ff0c1e10bd0016139744ef5404f4eb4d069709f9831f6de74a094944bf0f1c5bf89109e9855290336a66420376f

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <System/Cursor.hpp>
#include <Types/Togglable.hpp>

namespace obe
{
    namespace Editor
    {
        /**
         * \brief A grid for precise edition in the map editor
         * @Bind
         */
        class EditorGrid : public Types::Togglable
        {
        private:
            unsigned int m_gridSizeX;
            unsigned int m_gridSizeY;
            int gridOffX = 0;
            int gridOffY = 0;
            int gridMagnetX = -1;
            int gridMagnetY = -1;
        public:
            /**
             * \brief Constructor of the EditorGrid
             * \param cellWidth Width of a cell (in Pixels)
             * \param cellHeight Height of a cell (in Pixels)
             * \param offsetX Horizontal offset of the first displayed Cell (TopLeft)
             * \param offsetY Vertical offset of the first displayed Cell (TopRight)
             */
            explicit EditorGrid(unsigned int cellWidth, unsigned int cellHeight, int offsetX = 0, int offsetY = 0);
            /**
             * \brief Set the Width of a Cell
             * \param cellWidth Width of a cell (in Pixels)
             */
            void setCellWidth(unsigned int cellWidth);
            /**
             * \brief Set the Height of a Cell
             * \param cellHeight Height of a cell (in Pixels)
             */
            void setCellHeight(unsigned int cellHeight);
            /**
             * \brief Set the size of a Cell
             * \param cellWidth Width of a cell (in Pixels)
             * \param cellHeight Height of a cell (in Pixels)
             */
            void setSize(unsigned int cellWidth, unsigned int cellHeight);
            /**
             * \brief Set the horizontal offset of the first displayed Cell (TopLeft)
             * \param offsetX Horizontal offset of the first displayed Cell (TopLeft)
             */
            void setOffsetX(int offsetX);
            /**
            * \brief Set the vertical offset of the first displayed Cell (TopLeft)
            * \param offsetY Vertical offset of the first displayed Cell (TopLeft)
            */
            void setOffsetY(int offsetY);
            /**
             * \brief Set the offset of the first displayed Cell (TopLeft)
             * \param offsetX Horizontal offset of the first displayed Cell (TopLeft)
             * \param offsetY Vertical offset of the first displayed Cell (TopLeft)
             */
            void setOffset(int offsetX, int offsetY);
            /**
             * \brief Get the width of a Cell of the Grid (in Pixels)
             * \return Width of a Cell of the Grid (in Pixels)
             */
            unsigned int getCellWidth() const;
            /**
            * \brief Get the height of a Cell of the Grid (in Pixels)
            * \return Height of a Cell of the Grid (in Pixels)
            */
            unsigned int getCellHeight() const;
            /**
             * \brief Get the horizontal offset of the first displayed Cell (TopLeft)
             * \return Horizontal offset of the first displayed Cell (TopLeft) (in Pixels)
             */
            int getOffsetX() const;
            /**
            * \brief Get the vertical offset of the first displayed Cell (TopLeft)
            * \return Vertical offset of the first displayed Cell (TopLeft) (in Pixels)
            */
            int getOffsetY() const;
            /**
             * \brief Magnetize the System::Cursor to the closest Grid intersection
             * \param cursor Reference to the System::Cursor you want to magnetize
             */
            void magnetize(System::Cursor& cursor) const;
            /**
             * \brief Move the virtual magnet on the grid (Used to magnetize the System::Cursor)
             * \param cursor Reference to the System::Cursor you want to magnetize
             * \param x x Coordinate of the index of the intersection you want to be magnetized to
             * \param y y Coordinate of the index of the intersection you want to be magnetized to
             */
            void moveMagnet(System::Cursor& cursor, int x, int y);
            /**
             * \brief Draws the Grid on the Screen
             * \param target Screen you want to render the Grid on
             * \param cursor System::Cursor used to check which lines to highlight
             * \param offsetX x Coordinate of the drawing offset (Camera)
             * \param offsetY y Coordinate of the drawing offset (Camera)
             */
            void draw(sf::RenderWindow& target, System::Cursor& cursor, int offsetX, int offsetY);
        };
    }
}
