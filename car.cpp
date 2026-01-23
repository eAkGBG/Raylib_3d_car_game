#include "car.hpp"
#include <string>
#include <iostream>

float Car::maxSpeed = 0.3f;
float Car::minSpeed = -0.2f;

void Car::setWorld(PhysicsWorld* world){
    physWorld = world;
}
void Car::rotate(const float& delta){

    angle.y += delta;
    if(angle.y >= TWO_PI){
        angle.y -= TWO_PI;
    }
    else{
        if(angle.y < 0.0f) angle.y += TWO_PI;
    }

}

void Car::updateTransformation(){
    //translate first
    //position.y = physicsObject->position.y;
    Quaternion carRotate = QuaternionFromAxisAngle((Vector3){ 0.0f, 1.0f, 0.0f }, angle.y);
    //lets add a temp fix for the physics to know about the car's rotation.
    physicsObject.rotation = carRotate;
    carModel.transform = MatrixMultiply(QuaternionToMatrix(physicsObject.rotation),
                                    MatrixTranslate(physicsObject.position.x,
                                                    physicsObject.position.y,
                                                    physicsObject.position.z));
    physicsObject.UpdateWorldOBB();
    
    rayForward.position = physicsObject.position;
    rayBackward.position = physicsObject.position;
    rayLeft.position = physicsObject.position;
    rayRight.position = physicsObject.position;
    rayDownF.position = physicsObject.position;
    rayDownB.position = physicsObject.position;

    rayForward.position.y = physicsObject.position.y + 0.1f; //smal hack to handle elevation changes in track. we move the ray upp a small amount.
    rayBackward.position.y = physicsObject.position.y + 0.1f;
    rayLeft.position.y = physicsObject.position.y + 0.1f;
    rayRight.position.y = physicsObject.position.y + 0.1f;

    Matrix rotMat = QuaternionToMatrix(carRotate);
    Vector3 forward = {0.0f, 0.0f, 1.0f};
    Vector3 forwardWorld = Vector3Transform({0.0f, 0.0f, 1.0f}, rotMat);
    Vector3 backward = {0.0f, 0.0f, -1.0f};
    Vector3 left = {1.0f, 0.0f, 0.0f};
    Vector3 right = {-1.0f, 0.0f, 0.0f};
    Vector3 down = {0.0f, -1.0f, 0.0f};
    Vector3 downWorld = Vector3Transform(down, rotMat);
    
    rayForward.direction = Vector3Transform(forward, rotMat);
    rayBackward.direction = Vector3Transform(backward, rotMat);
    rayLeft.direction = Vector3Transform(left, rotMat);
    rayRight.direction = Vector3Transform(right, rotMat);
    rayDownF.direction = downWorld;
    rayDownB.direction = downWorld;
    rayDownF.position = Vector3Add(physicsObject.position, Vector3Scale(forwardWorld, 0.5f));
    rayDownB.position = Vector3Add(physicsObject.position, Vector3Scale(forwardWorld, -0.5f));
    rayDownF.position.y = physicsObject.position.y + 0.3f;
    rayDownB.position.y = physicsObject.position.y + 0.3f;
}
