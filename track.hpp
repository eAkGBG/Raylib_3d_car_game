#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

struct GameLines{
    Vector3 startLine;
    Vector3 startLineB; //B for bottom
    Vector3 endLine;
    Vector3 endLineB;
    Color randomColor;
};

class GameTrack {
    public:
        std::vector<GameLines> innerBarrier;
        std::vector<GameLines> outerBarrier;
        int trackSize;
        int segmentDegrees;

        //Constructor
        GameTrack(int size = 5, int segDeg = 10)
            : trackSize(size), segmentDegrees(segDeg){}
            
        void BuildTrack();
};
