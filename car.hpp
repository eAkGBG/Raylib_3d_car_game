#pragma once

#include "raylib.h"
#include "raymath.h"
#include "physics.hpp"
#include <vector>
#include <iostream>

class Car{
    public:
        Model carModel; 
        Vector3 position;
        Vector3 size;
        Vector3 angle;
        float speed;
        float mass;
        Ray rayForward;
        Ray rayBackward;
        Ray rayLeft;
        Ray rayRight;
        Ray rayDownF;
        Ray rayDownB;
        Object physicsObject;
        
        Car (Vector3 setSize = {0.5f, 0.4f, 1.0f}, Vector3 setPosition = {0.0f, 0.5f, 0.0f}, PhysicsWorld* world = nullptr)
            : position (setPosition), size(setSize),
            angle({0.0f,0.0f,0.0f}),
            speed(0.0f),
            mass(0.1f),
            rayForward(setPosition, {0.0f, 0.25f, 0.0f}), //a little hack point the rays slightly upp to compensate for elevation changes in the track.
            rayBackward(setPosition, {-1.0f, 0.25f, 0.0f}),
            rayLeft(setPosition, {0.0f, 0.25f, 1.0f}),
            rayRight(setPosition, {0.0f, 0.25f, -1.0f}),
            rayDownF(setPosition, {0.0f,-1.0f,0.0f}),
            rayDownB(setPosition, {0.0f,-1.0f,0.0f}),
            physWorld(world)
        {
            //Mesh cubeMesh = GenMeshCube(size.x,size.y,size.z);
            carModel = LoadModel("./models/car.glb");
            physicsObject.mass = 1200.0f;
            physicsObject.position = position;
            physicsObject.force = {0.0f, 0.0f, 0.0f};
            physicsObject.velocity = {0.0f, 0.0f, 0.0f};
            if(physWorld){
                physWorld->AddDynObject(&physicsObject);
            }else{
                std::cout << "No Physics World!!" << std::endl;
            }
        }

        void translate(const float& power, const float& dt);
        void rotate(const float& delta);
        void updateRays();
        void updateTransformation();
        void gravity(const RayCollision& cA, const RayCollision& cB, const float& dt);
        void setWorld(PhysicsWorld* world);
    private:
        static constexpr float TWO_PI = 2.0f * PI;
        static float maxSpeed;
        static float minSpeed;
        PhysicsWorld* physWorld;
};