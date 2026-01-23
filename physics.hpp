/*
The awsome raylib physics engine!
*/

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
    Quaternion rotation;   // How the box is rotated
};

/* ----------  What an Object is  ---------- */
struct Object{
    Vector3 position = { 0.0f, 0.0f, 0.0f }; // Where the object is
    Vector3 velocity = { 0.0f, 0.0f, 0.0f }; // How fast it’s moving (in each direction)
    Vector3 force = { 0.0f, 0.0f, 0.0f };    // Forces acting on it (gravity, pushes, etc.)
    Quaternion rotation{0, 0, 0, 1}; // Current rotation
    float mass = 1.0f;       // How heavy it is

    //Keep our OBB box here.
    Vector3 localOffset = { 0.0f, 0.0f, 0.0f };
    OBB worldOBB;

    void UpdateWorldOBB(){
        Vector3 rotatedOffset = Vector3RotateByQuaternion(localOffset, rotation);
        worldOBB.center = Vector3Add(position, rotatedOffset);
        worldOBB.rotation = rotation;
    }
    void BuildObbFromModel(Model* model){
            if (model->meshCount == 0){
                std::cout << "Error no mesh found in model!" << std::endl;
                return;
            };
            //initialize with a big number to calculate vertex positions to find the OBB size.
            Vector3 min = {1000000.0f, 1000000.0f, 1000000.0f};
            Vector3 max = {-1000000.0f, -1000000.0f, -1000000.0f};

            Mesh mesh = model->meshes[0];
            if(mesh.vertices != nullptr){
                int vertexCount = mesh.vertexCount;
                Vector3 *vertData = (Vector3 *)mesh.vertices;
                for (int i = 0; i < vertexCount; i++){
                    Vector3 v = vertData[i];
                    if(v.x < min.x) min.x = v.x;
                    if(v.y < min.y) min.y = v.y;
                    if(v.z < min.z) min.z = v.z;

                    if(v.x > max.x) max.x = v.x;
                    if(v.y > max.y) max.y = v.y;
                    if(v.z > max.z) max.z = v.z;
                }
                float widthHalf = (max.x - min.x)*0.5f;
                float heightHalf = (max.y - min.y)*0.5f;
                float lengthHalf = (max.z - min.z)*0.5f;

                worldOBB.halfSize = {widthHalf, heightHalf, lengthHalf};
                localOffset = { min.x + widthHalf, min.y + heightHalf, min.z + lengthHalf};
                UpdateWorldOBB();
            }
            
        }
};

class PhysicsWorld {
    private:
        std::vector<Object*> dynObjects;   // Objects that can move
        std::vector<Model*> staticObjects; // Objects that stay still (Scenery objects for example the ground)
        
        Vector3 w_gravity = (Vector3){0.0f, -9.8f, 0.0f};
        void ScanCollisions(){ //Depricated replaced with OBB collision physics.
            if (staticObjects.empty()) {
                std::cout << "DEBUG: staticObjects is empty!!" << std::endl;
                return;
            }
            for (Object* obj : dynObjects){
                Ray down;
                down.position = obj->position;
                down.position.y += 0.2; //for now quick hack to detect floor.
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
                        obj->position.y = cD.point.y + 0.01f; //Add som height to be safe.
                        if(obj->velocity.y < 0) obj->velocity.y = 0; // we take out the downwards velocity from G when on the road.
                        groundFound = true;
                    }
                }
                if (!groundFound) {
                    //std::cout << "Warning no Ray hit!!" << std::endl;
                }else{
                    //std::cout << "Ray hit!!" << std::endl;
                }
            }
        }
        void ObbCollisions(){
            if (staticObjects.empty()) {
                std::cout << "DEBUG: staticObjects is empty!!" << std::endl;
                return;
            }
             //add auto calculated rays from Object OBB.
            Vector3 directions[] = {
                {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, // Right, Left
                {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, // Upp, Down
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} // Forward, Backward
            };
            const int numRays = sizeof(directions)/sizeof(Vector3);
            for (Object* obj : dynObjects){
                Ray currentObjectRays[numRays];
                for(int i = 0; i < numRays; i++){
                    //Build each ray.
                    currentObjectRays[i].position = obj->worldOBB.center;
                    currentObjectRays[i].direction = Vector3RotateByQuaternion(directions[i], obj->rotation);
                };
                
                for(Model* staticMesh : staticObjects){
                    // do a collision for each ray and calculate it.
                    for(int i = 0; i < numRays; i++){
                        float dt = GetFrameTime();
                        // Predict future position and send a ray that way to make our collisions see in the future and adapt.
                        Vector3 deltaMovement = Vector3Scale(obj->velocity, dt);
                        //Vector3 futurePosition = Vector3Add(obj->worldOBB.center, deltaMovement);

                        float moveDistInDirection  = Vector3DotProduct(deltaMovement, currentObjectRays[i].direction);
                        if(moveDistInDirection < 0.0f){moveDistInDirection = 0;};

                        Ray deltaRay;
                        deltaRay.position = obj->worldOBB.center;
                        deltaRay.direction = Vector3Normalize(obj->velocity);
                        float deltaDistance = Vector3Length(deltaMovement);

                        RayCollision c = GetRayCollisionMesh(currentObjectRays[i], staticMesh->meshes[0], staticMesh->transform);
                        if(c.hit){
                            if(i == 0 || i == 1){ //left/right
                                if(c.distance < obj->worldOBB.halfSize.x + moveDistInDirection){StaticCollisionHelper(obj, &c);};
                            }
                            if(i == 2 || i == 3){ //up/down
                                if(c.distance < obj->worldOBB.halfSize.y + moveDistInDirection){StaticCollisionHelper(obj, &c);};
                            }
                            if(i == 4 || i == 5){ //front/back
                                if(c.distance < obj->worldOBB.halfSize.z + moveDistInDirection){StaticCollisionHelper(obj, &c);};
                            }
                        }                        
                    }
                }
            }
        }
        void StaticCollisionHelper(Object* obj, RayCollision* c){   //Todo we want to use propotional forces.
                                                                    //instead of manipulating the velocity.
            float pushDistance = 0.05f;
            obj->position = Vector3Add(obj->position, Vector3Scale(c->normal, pushDistance));
            float dot = Vector3DotProduct(obj->velocity, c->normal);
            if(dot < 0.0f) {
                Vector3 velocityIntoMesh = Vector3Scale(c->normal, dot);
                obj->velocity = Vector3Subtract(obj->velocity, velocityIntoMesh);
                //obj->velocity = Vector3Scale(obj->velocity, 0.98f); //friction ?
            }
            obj->UpdateWorldOBB();

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
        
        void Accelerate(Object* object, Vector3 direction, float force){
            Vector3 worldDirection = Vector3RotateByQuaternion(direction, object->rotation);
            object->force = Vector3Add(object->force, Vector3Scale(worldDirection, force));
        }
        void Rotate(Object* object, Vector3 direction){ //ToDo: Here ios big bugg. we did not check movement direction.
                                                        //so if rewerse the car stopps. But wee need rewrite to rotate with force instead annyways.
            //Hacks to make it turn we can't simply rotate the velocity vector. TODO:
            //Make the function so that it is universal and work on the force so we can slide.
            float tempVelocity = Vector3Length(object->velocity);
            Vector3 localForward = Vector3RotateByQuaternion({0,0,1}, object->rotation); // use the local Z directon and object rotation quaternion. i 
            object->velocity = Vector3Scale(localForward, tempVelocity);
        }
        //void Gravity(Object* object, float dt)
        void Gravity(Object* object){
            //force = force + (gravity × mass)
            object->force = Vector3Add(object->force,
                                    Vector3Scale(w_gravity, object->mass));
        }
        /* ----------  One frame of physics  ---------- */
        void Step(float dt){
            for (Object* obj : dynObjects){
                //Apply Gravity.
                //Gravity(obj, dt);
                Gravity(obj);
                //Acumulator Velocity. Here we build the velocity vector.
                //a = F / m
                //Acceleration = Force ÷ Mass
                Vector3 accel = Vector3Scale(obj->force, 1.0f / obj->mass);
                //v = v + (a × dt)
                obj->velocity = Vector3Add(obj->velocity,
                                            Vector3Scale(accel, dt));
                //p = p + (v × dt)
                obj->position = Vector3Add(obj->position,
                                            Vector3Scale(obj->velocity, dt));
                //Acumulator Rotation. Here we build the rotatinal force TODO: write some code here.
                
                                //Run our collision handler.
                ObbCollisions();
                
                //Clear forces before next frame.
                obj->force = {0.0f, 0.0f, 0.0f};
            }
        }
};