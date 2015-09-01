#pragma once

class HSVColor
{
public:
    HSVColor()
        : HSVColor(0, 0, 0)
    {
    }

    HSVColor(int h, int s, int v)
    {
        H = h;
        S = s;
        V = v;
    }

    int H;
    int S;
    int V;
};
