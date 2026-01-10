// A simple raylib test app.

#include "raylib.h"
#include "raymath.h"
#include <string>
#include <iostream>
#include <vector>
#include "track.hpp"
#include "car.hpp"

const int SCREEN_SIZE = 720;
//Calculate play feild grid.
const int GRID_COLS = 10;
const int GRID_ROWS = 10;

bool game_checkCollision(BoundingBox &player, BoundingBox &enemy){
    return CheckCollisionBoxes(player, enemy);
}
bool game_trackCollisions(Car &car, Model &track, float &dt, std::string &hitObjectName){
    RayCollision cF = GetRayCollisionMesh(car.rayForward, track.meshes[0], track.transform);
    RayCollision cB = GetRayCollisionMesh(car.rayBackward, track.meshes[0], track.transform);
    RayCollision cL = GetRayCollisionMesh(car.rayLeft, track.meshes[0], track.transform);
    RayCollision cR = GetRayCollisionMesh(car.rayRight, track.meshes[0], track.transform);
    RayCollision cG = GetRayCollisionMesh(car.rayDown, track.meshes[0], track.transform); //ground
    
    if(cG.hit){
        car.gravity(cG);
    }
    
    if(cF.hit && cF.distance < 1.0f){
        if(cF.distance < 0.8f){
            car.speed *= 0.2f;
            car.translate(-0.3f * dt, dt);
        }else {
            car.speed *= 0.2f;
            car.translate(-0.2f * dt, dt);
        }
        hitObjectName = (std::string) "Ray forward";
        return true;
    }
    if(cB.hit && cB.distance < 0.6f){
        if(cB.distance < 0.3f){
            car.speed *= 0.2f;
            car.translate(0.3f * dt, dt);
        }else {
            car.speed *= 0.2f;
            car.translate(0.2f * dt, dt);
        }
        hitObjectName = (std::string) "Ray backward";
        return true;
    }
    if(cL.hit && cL.distance < 0.5f){
        car.speed *= 0.5f;
        hitObjectName = (std::string) "Ray left";
        return true;
    }
    if(cR.hit && cR.distance < 0.5f){
        car.speed *= 0.8f;
        hitObjectName = (std::string) "Ray right";
        return true;
    }

    return false;
}

int main(void)
{
    const int screenWidth  = SCREEN_SIZE;
    const int screenHeight = SCREEN_SIZE;
    InitWindow(screenWidth, screenHeight, "Raylib basic window"); //Important to call init window before doing calculations on font and probably other stuff!
    SetTargetFPS(60);
 
    //use my new shiny car class
    Car myCar;

    //load game world
    Model road = LoadModel("./models/road.glb");
    Model sky = LoadModel("./models/sky.glb");
    Model ground = LoadModel("./models/ground.glb");

    // Init camera view in 3d scene.
    Camera3D camera = { 0 }; 
    camera.position = (Vector3){0.0f, 20.0f, 30.0f };  //Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};        //Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};            //Camera up vector (rotation need to figure this out but this should be pointing at target)
    camera.fovy = 90.0f;                                //camera FOV Y
    camera.projection = CAMERA_PERSPECTIVE;             //Camera projection type.

    //text effect
    //ok how to solve make posY this will increase with if statement untill we reach 20 timesteps. then it will decrease for 20 timesteps.
    //and start ower. we need posX this will decrease from GetScreenWidth() + amount of chars in string?
    //and start ower. maby we need to create a array struct that holds each char and it's screen space x/y we will update it with the if statements
    //over timesteps. and call DrawText("", x,y,20) for each struct in the array.
    struct FxText{
        int x;
        int y;
        char letter[2];
        int letterWidth;
        bool reverseFxY;
    };
    char fxTextToUse[] = "The awsome Raylib 3D test!";
    int arrayLength = sizeof(fxTextToUse)/sizeof(char);
    //std::cout << "Array length is: " << arrayLength << std::endl;
    struct FxText *fxArray = (struct FxText*) malloc(arrayLength * sizeof(struct FxText));
    if(fxArray == NULL){
        std::cout << "Failed allocation!" << std::endl;
        return 1;
    }else {
        for (int i = 0; i < arrayLength; i++){
            fxArray[i].letter[0] = fxTextToUse[i];
            fxArray[i].letter[1] = '\0';
            fxArray[i].letterWidth = (MeasureText(fxTextToUse, 20)/(arrayLength -1)) + 5;
            fxArray[i].x = GetScreenWidth() + (i*fxArray[i].letterWidth);
            fxArray[i].y = (GetScreenHeight()/2) + sinf(i*0.01)*40;
            fxArray[i].reverseFxY = false;
            //std::cout << fxArray[i].letter << " y: " << fxArray[i].y << " letterWidth: " << fxArray[i].letterWidth << std::endl;
        }
    }
    //bool reverseFxY = false;
    int textCounter = 0;
    float fxTimer = 0.0f;
    
    while (!WindowShouldClose())        
    {
        float dt = GetFrameTime(); //Keep track of time for deltaTime functionality.
        
        if(IsKeyDown(KEY_W)){
            myCar.translate(0.5f * dt, dt);
        }
        if(IsKeyDown(KEY_S)){
            myCar.translate(-0.5f * dt, dt);
        }
        if(IsKeyDown(KEY_A)){
            myCar.rotate(3.0f*dt);
        }
        if(IsKeyDown(KEY_D)){
            myCar.rotate(-3.0f*dt);
        }

        if(!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)){
            myCar.translate(0.0f,dt);
        }

        
        
        // let's look for collisions.
        std::string hitObjectName = "None"; // this one used to siplay collision information.
        game_trackCollisions(myCar, road, dt, hitObjectName);
        
        //after collisions we update cars transformation
        myCar.updateTransformation();


        //FX TEXT Parsing
        fxTimer += dt;
        if(fxTimer > 0.015){
            for(textCounter = 0; textCounter < arrayLength -1; textCounter++){
                //std::cout << " inside loop!" << std::endl;
                if(fxArray[textCounter].x > 0){
                    //std::cout << " inside loop! x: " << fxArray[textCounter].x << std::endl;
                    fxArray[textCounter].x-=2;
                }
                if(fxArray[textCounter].x <= 0){
                    fxArray[textCounter].x = (GetScreenWidth() + fxArray[textCounter].letterWidth);
                }
                    fxArray[textCounter].y = (GetScreenHeight()/2) + (sinf(textCounter*0.05 + (fxArray[textCounter].x * 0.025))*20);                
            }
            fxTimer = 0.0f;
        }
        camera.position = Vector3Add(myCar.position, {0.0f, 15.0f, -20.0f});
        camera.target = myCar.position;
        
        //UpdateCamera(&camera, CAMERA_ORBITAL);
        //Draw scene
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawModel(road,(Vector3){0.0f,0.0f,0.0f},1.0f, WHITE);
                DrawModel(ground,(Vector3){0.0f,0.0f,0.0f},1.0f, WHITE);
                DrawModel(sky,(Vector3){0.0f,4.5f,0.0f},1.0f, WHITE);
                
                DrawRay(myCar.rayForward, GREEN);
                DrawRay(myCar.rayLeft, BLUE);
                DrawRay(myCar.rayRight, ORANGE);
                DrawRay(myCar.rayBackward, RED);
                DrawModel(myCar.carModel,(Vector3){0.0f,0.0f,0.0f},1.0f, RED);


                //DrawGrid(100, 1.0f); //first part is amount of grid squares. second part is the unit size exampel 1.0f is 1x1 cell size. 
            EndMode3D();
            //FxText rendering.
            for(int i = 0; i < arrayLength - 1; i++){
            DrawText(fxArray[i].letter, fxArray[i].x, fxArray[i].y, 20, DARKPURPLE);
            }
            DrawText(hitObjectName.c_str(), 10,40,20,DARKGREEN);
            DrawFPS(10,10); //allways nice to se fps.
        EndDrawing();
        
    }
    //wonder if there is more stuff you should unload?
    //UnloadModelAnimations(modelAnimations, animationsCount);
    UnloadModel(myCar.carModel);
    UnloadModel(road);
    free(fxArray);
    //free(lineArray);
    //UnloadTexture(gubbenTex);
    CloseWindow();
    return 0;
}
