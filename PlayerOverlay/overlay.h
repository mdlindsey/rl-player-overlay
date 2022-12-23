#pragma once
#include <iostream>
#include <vector>
#include "global.h"

using namespace std;

// set size, location, columns/width, etc

class Overlay
{

    public:
    static enum class UnitType { Pixel, Percent };
    static enum class Position { TopLeft, TopMid, TopRight, MidRight, BottomRight, BottomMid, BottomLeft, MidLeft, MidMid };
    
    static inline int ScreenWidth = 1920;
    static inline int ScreenHeight = 1080;
    static int WidthPx(int units, UnitType unitType)
    {
        switch(unitType)
        {
            case UnitType::Percent:
                return Overlay::ScreenWidth * (units / 100);
            case UnitType::Pixel:
            default:
                return units;
        }
    }
    static int HeightPx(int units, UnitType unitType)
    {
        switch(unitType)
        {
            case UnitType::Percent:
                return Overlay::ScreenHeight * (units / 100);
            case UnitType::Pixel:
            default:
                return units;
        }
    }

    CanvasWrapper *canvas;
    Position position = Position::MidMid;

    int startX;
    int startY;

    int widthUnits;
    int heightUnits;
    UnitType widthUnitType;
    UnitType heightUnitType;


    int curX;
    int curY;
    int lineHeightPx = 14;

    Overlay(CanvasWrapper canvas)
    {
        this->canvas = &canvas;
        this->SetScreenSize();
    }

    void SetPosition(Position position)
    {
        this->position = position;
    }

    void SetWidth(int units, UnitType unitType = Overlay::UnitType::Pixel)
    {
        this->widthUnits = units;
        this->widthUnitType = unitType;
    }

    void SetHeight(int units, UnitType unitType = Overlay::UnitType::Pixel)
    {
        this->heightUnits = units;
        this->heightUnitType = unitType;
    }

    void SetColor(int red, int green, int blue, int alpha)
    {
        this->canvas->SetColor(LinearColor{ float(red), float(green), float(blue), float(alpha) });
    }

    void NextLine()
    {
        this->curX = this->startX;
        this->curY += this->lineHeightPx;
    }
    void Print(string str, int widthPx = 0)
    {
        this->canvas->SetPosition(Vector2F{ float(this->curX), float(this->curY) });
        // the two floats are text x and y scale, false turns off the drop shadow
        this->canvas->DrawString(str, 1.0, 1.0, false);
        this->curX += widthPx;
    }

    void Render()
    {
        this->UpdatePosition();
        int widthPx = Overlay::WidthPx(this->widthUnits, this->widthUnitType);
        int heightPx = Overlay::HeightPx(this->heightUnits, this->heightUnitType);
        int endX = this->startX + widthPx;
        int endY = this->startY + heightPx;
	    this->canvas->DrawRect(Vector2F{ float(this->startX), float(this->startY) }, Vector2F{ float(endX), float(endY) });
        this->startX += 5; // padding
        this->startY += 5; // padding

        this->curX = this->startX;
        this->curY = this->startY;
    }

    private:
    void SetScreenSize()
    {
        Vector2 canvasSize = this->canvas->GetSize();
        Overlay::ScreenWidth = canvasSize.X;
        Overlay::ScreenHeight = canvasSize.Y;
    }
    void UpdatePosition()
    {
        int bottomPadding = 0; // 45px for taskbar/ballcam alignment?
        int widthPx = Overlay::WidthPx(this->widthUnits, this->widthUnitType);
        int heightPx = Overlay::HeightPx(this->heightUnits, this->heightUnitType);
        switch(this->position)
        {
            case Position::BottomMid:
            default:
                this->startX = (Overlay::ScreenWidth - widthPx) / 2;
                this->startY = Overlay::ScreenHeight - heightPx - bottomPadding;
                break;
        }
    }
};
