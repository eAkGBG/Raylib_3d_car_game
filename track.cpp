
#include "track.hpp"

void GameTrack::BuildTrack() {
    innerBarrier.clear(); //building new track clear everything.
    outerBarrier.clear();

    int segments = 360 / segmentDegrees;

    for(int i = 0; i < segments; ++i){
        float angleRadian = DEG2RAD * i * segmentDegrees;
        GameLines lineSegInner;
        GameLines lineSegOuter; 
        lineSegInner.startLine = {trackSize * cosf(angleRadian), 2.0f, trackSize * sinf(angleRadian)};
        lineSegInner.startLineB = lineSegInner.startLine;
        lineSegInner.startLineB.y -= 2.0f;
        lineSegInner.endLine ={trackSize * cosf(angleRadian+(DEG2RAD * segmentDegrees)), 2.0f, trackSize * sinf(angleRadian+(DEG2RAD * segmentDegrees))};
        lineSegInner.endLineB = lineSegInner.endLine;
        lineSegInner.endLineB.y -= 2.0f;
        lineSegInner.randomColor = {
            (unsigned char)GetRandomValue(0,255), //red
            (unsigned char)GetRandomValue(0,255), //green
            (unsigned char)GetRandomValue(0,255), //blue
            (unsigned char)255
        };
        lineSegOuter.startLine = {(trackSize + 5) * cosf(angleRadian), 2.0f, (trackSize + 5) * sinf(angleRadian)};
        lineSegOuter.startLineB = lineSegOuter.startLine;
        lineSegOuter.startLineB.y -= 2.0f;
        lineSegOuter.endLine ={(trackSize + 5) * cosf(angleRadian+(DEG2RAD * segmentDegrees)), 2.0f, (trackSize + 5) * sinf(angleRadian+(DEG2RAD * segmentDegrees))};
        lineSegOuter.endLineB = lineSegOuter.endLine;
        lineSegOuter.endLineB.y -= 2.0f;
        lineSegOuter.randomColor = lineSegInner.randomColor;
        
        innerBarrier.push_back(std::move(lineSegInner));
        outerBarrier.push_back(std::move(lineSegOuter));
    }
}