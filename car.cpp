#include "car.hpp"
#include <string>
#include <iostream>

float Car::maxSpeed = 0.3f;
float Car::minSpeed = -0.2f;

void Car::translate(const float& power, const float& dt){
    speed += power; //Accell/decel power applied to speed.
    if(power == 0.0f){  //Ad some friction to make it stop.
        if(speed > 0.01f) speed -= 0.15 * dt;
        else if(speed < -0.01f) speed += 0.15 * dt;
        else speed = 0.0f;
    }
    if(speed > maxSpeed) speed = maxSpeed;
    if(speed < minSpeed) speed = minSpeed;
    
    position.x += sinf(angle.y) * speed;
    position.z += cosf(angle.y) * speed;

}
void Car::rotate(const float& delta){

    angle.y += delta;
    if(angle.y >= TWO_PI) angle.y -= TWO_PI;
    if(angle.y < 0.0f) angle.y += TWO_PI;

}
void Car::gravity(const RayCollision& cA, const RayCollision& cB, const float& dt){
    //ok i need to fix this. i have some thougts. or i need to read a book about
    //game physics. and implement some simple fysics to make the car jump on hills.
    //atleast now it seems to fall from hills.

    Vector3 hit_position = cA.point; //lets work with the position hit were found.
    //std::cout << "Y hit at: " << std::to_string(hit_position.y) << " Distance from ground: " << std::to_string(c.distance) << std::endl;
    
    //try to create some gravity.
    if(cA.distance < 0.2f && cA.normal.y < 0.98f && speed > 0.0f && cA.distance < cB.distance){
        gravityVelocityY += 100.0f * speed;
        std::cout << "let's jump now!" << std::endl;
    }else{
        gravityVelocityY = 0.0f;
    }
    gravityVelocityY += (mass * -9.8f);
    position.y += gravityVelocityY * dt;

    if(position.y < hit_position.y){
        position.y = hit_position.y + 0.0f;
        if(cB.distance < cA.distance ){
            gravityVelocityY = 0.0f;
        }

    }
    
    previousY = position.y;   
}

void Car::updateTransformation(){
    //translate first
    Quaternion carRotate = QuaternionFromAxisAngle((Vector3){ 0.0f, 1.0f, 0.0f }, angle.y);
    carModel.transform = MatrixMultiply(QuaternionToMatrix(carRotate),
                                    MatrixTranslate(position.x,
                                                    position.y,
                                                    position.z));
    
    rayForward.position = position;
    rayBackward.position = position;
    rayLeft.position = position;
    rayRight.position = position;
    rayDownF.position = position;
    rayDownB.position = position;

    rayForward.position.y = position.y + 0.1f; //smal hack to handle elevation changes in track. we move the ray upp a small amount.
    rayBackward.position.y = position.y + 0.1f;
    rayLeft.position.y = position.y + 0.1f;
    rayRight.position.y = position.y + 0.1f;

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
    rayDownF.position = Vector3Add(position, Vector3Scale(forwardWorld, 0.5f));
    rayDownB.position = Vector3Add(position, Vector3Scale(forwardWorld, -0.5f));
    rayDownF.position.y = position.y + 0.3f;
    rayDownB.position.y = position.y + 0.3f;
}
