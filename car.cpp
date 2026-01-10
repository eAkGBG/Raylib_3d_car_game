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
void Car::gravity(const RayCollision& c){
    Vector3 hit_position = c.point; //lets work with the position hit were found.
    //std::cout << "Y hit at: " << std::to_string(hit_position.y) << " Distance from ground: " << std::to_string(c.distance) << std::endl;
    if(position.y - 0.1f < hit_position.y){
        position.y = hit_position.y + 0.1f;
    }
    //lets do some crude test gravity.
    if(position.y -0.1f > hit_position.y){
        position.y = hit_position.y + 0.1f;
    }
}

void Car::updateTransformation(){
    //rotation = MatrixRotateY(angle.y);
    //translation = MatrixTranslate(position.x, position.y, position.z);
    //carModel.transform = MatrixMultiply(rotation, translation);
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
    rayDown.position = position;

    rayForward.position.y = position.y + 0.3f;
    rayBackward.position.y = position.y + 0.3f;
    rayLeft.position.y = position.y + 0.3f;
    rayRight.position.y = position.y + 0.3f;
    rayDown.position = position;

    Matrix rotMat = QuaternionToMatrix(carRotate);
    Vector3 forward = {0.0f, 0.0f, 1.0f};
    Vector3 backward = {0.0f, 0.0f, -1.0f};
    Vector3 left = {1.0f, 0.0f, 0.0f};
    Vector3 right = {-1.0f, 0.0f, 0.0f};
    //Vector3 down = {0.0f, -1.0f, 0.0f}; // no need to recalculate down ray rotation for now.
    rayForward.direction = Vector3Transform(forward, rotMat);
    rayBackward.direction = Vector3Transform(backward, rotMat);
    rayLeft.direction = Vector3Transform(left, rotMat);
    rayRight.direction = Vector3Transform(right, rotMat);
}
