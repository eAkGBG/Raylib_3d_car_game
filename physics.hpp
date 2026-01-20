//I asked gptoss20b to comment the mathcode for a 5th grader.

#pragma once

#include <vector>
#include <algorithm>
#include "raylib.h"
#include "raymath.h"
#include <iostream>

/* ----------  What an OBB is  ---------- */
// OBB = Oriented Bounding Box (a fancy way to say “a box that can rotate”)
struct OBB {
    Vector3 center;   // Where the box is in space
    Vector3 halfSize; // Half the width, height, depth
    Quaternion rot;   // How the box is rotated
};

/* ----------  What an Object is  ---------- */
struct Object{
    Vector3 position = { 0.0f, 0.0f, 0.0f }; // Where the object is
    Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // How fast it’s moving (in each direction)
    Vector3 force = { 0.0f, 0.0f, 0.0f };    // Forces acting on it (gravity, pushes, etc.)
    Quaternion rotation{0, 0, 0, 1}; // Current rotation
    float mass = 1.0f;       // How heavy it is
};

class PhysicsWorld {
    private:
        std::vector<Object*> dynObjects;   // Objects that can move
        std::vector<Model*> staticObjects; // Objects that stay still (Scenery objects for example the ground)
        
        Vector3 w_gravity = (Vector3){0.0f, -9.8f, 0.0f};
        void ScanCollisions(){
            if (staticObjects.empty()) {
                std::cout << "DEBUG: staticObjects is empty!!" << std::endl;
                return;
            }
            for (Object* obj : dynObjects){
                Ray down;
                down.position = obj->position;
                down.position.y += 2; //for now quick hack to detect floor.
                down.direction = {0.0f,-1.0f,0.0f};
                
                //lets do some safty teleport up if we go under.
                Ray upp;
                upp.position = obj->position;
                upp.position.y -= 1; //for now quick hack to detect floor.
                upp.direction = {0.0f,1.0f,0.0f};

                bool groundFound = false;

                for(Model* staticMesh : staticObjects){
                    RayCollision cD = GetRayCollisionMesh(down, staticMesh->meshes[0], staticMesh->transform);
                    RayCollision cU = GetRayCollisionMesh(upp, staticMesh->meshes[0], staticMesh->transform);
                    if(cD.hit && cD.point.y > obj->position.y){
                        obj->position.y = cD.point.y;
                        if(obj->velocity.y < 0) obj->velocity.y = 0; // we take out the downwards velocity from G when on the road.
                        groundFound = true;
                    }
                    /* if(cU.hit && cU.point.y < obj->position.y){
                        obj->position.y = cU.point.y + 0.1f;
                        if(obj->velocity.y > 0) obj->velocity.y = 0;
                        groundFound = true;
                    }  */
                }
                if (!groundFound) {
                    //std::cout << "Warning no Ray hit!!" << std::endl;
                }else{
                    //std::cout << "Ray hit!!" << std::endl;
                }
            }
        }

    public:
        /* ----------  Add / Remove helpers  ---------- */
        void AddDynObject(Object* object){ dynObjects.push_back(object); }
        void RemoveDynObject(Object* object){
            dynObjects.erase(std::remove(dynObjects.begin(), dynObjects.end(), object),
                             dynObjects.end());
        }
        void AddStaticObject(Model* object){ staticObjects.push_back(object); }
        void RemoveStaticObject(Model* object){
            staticObjects.erase(std::remove(staticObjects.begin(), staticObjects.end(), object),
                                staticObjects.end());
        }

        /* ----------  One frame of physics  ---------- */
        void Step(float dt){
            for (Object* obj : dynObjects){

                //force = force + (gravity × mass)
                obj->force = Vector3Add(obj->force,
                                        Vector3Scale(w_gravity, obj->mass));

                //a = F / m
                //Acceleration = Force ÷ Mass
                Vector3 accel = Vector3Scale(obj->force, 1.0f / obj->mass);
                //v = v + (a × dt)
                obj->velocity = Vector3Add(obj->velocity,
                                           Vector3Scale(accel, dt));

                //p = p + (v × dt)
                obj->position = Vector3Add(obj->position,
                                           Vector3Scale(obj->velocity, dt));

                //Clear forces before next frame.
                obj->force = {0.0f, 0.0f, 0.0f};
                //Run our collision handler.
                ScanCollisions();
            }
        }
};