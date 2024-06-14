#pragma once
#include "Curve.h"
#include "Mesh.h"

class Bezier :
    public Curve
{
public:
    Bezier();
    void generateCurve(int pointsPerSegment);
};
