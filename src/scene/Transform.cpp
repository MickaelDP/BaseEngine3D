#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>


glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);  // identity

    // LIFO Order (Last In, First Out): Read this bottom-up. The LAST operation 
    // written in code is the FIRST applied to the vertex. Order matters enormously — 
    // scaling after translating would also scale the translation distance, sending 
    // the object somewhere unexpected.
    model = glm::translate(model, position);

    // Euler angles applied in Z, Y, X order (roll, yaw, pitch).
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));

    model = glm::scale(model, scale);

    return model;
}