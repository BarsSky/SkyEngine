#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


/**
 * @brief static function for calculate rotate around point
*/
static glm::mat4 rotateAroundPoint(float rad, const glm::vec3 &point, const glm::vec3 &axis)
{
    auto t1 = glm::translate(glm::mat4(1.0f), -point);
    auto r = glm::rotate(glm::mat4(1.0f), rad, axis);
    auto t2 = glm::translate(glm::mat4(1.0f), point);
    return t2 * r * t1;
}