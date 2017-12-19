//
//  AnimPose.cpp
//
//  Created by Anthony J. Thibault on 10/14/15.
//  Copyright (c) 2015 High Fidelity, Inc. All rights reserved.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "AnimPose.h"
#include <GLMHelpers.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

const AnimPose AnimPose::identity = AnimPose(glm::vec3(1.0f),
                                             glm::quat(),
                                             glm::vec3(0.0f));

AnimPose::AnimPose(const glm::mat4& mat) {
    static const float EPSILON = 0.0001f;
    _scale = extractScale(mat);
    // quat_cast doesn't work so well with scaled matrices, so cancel it out.
    glm::mat4 tmp = glm::scale(mat, 1.0f / _scale);
    _rot = glm::quat_cast(tmp);
    float lengthSquared = glm::length2(_rot);
    if (glm::abs(lengthSquared - 1.0f) > EPSILON) {
        float oneOverLength = 1.0f / sqrtf(lengthSquared);
        _rot = glm::quat(_rot.w * oneOverLength, _rot.x * oneOverLength, _rot.y * oneOverLength, _rot.z * oneOverLength);
    }
    _trans = extractTranslation(mat);
}

glm::vec3 AnimPose::operator*(const glm::vec3& rhs) const {
    return _trans + (_rot * (_scale * rhs));
}

glm::vec3 AnimPose::xformPoint(const glm::vec3& rhs) const {
    return *this * rhs;
}

// really slow, but accurate for transforms with non-uniform scale
glm::vec3 AnimPose::xformVector(const glm::vec3& rhs) const {
    glm::vec3 xAxis = _rot * glm::vec3(_scale.x, 0.0f, 0.0f);
    glm::vec3 yAxis = _rot * glm::vec3(0.0f, _scale.y, 0.0f);
    glm::vec3 zAxis = _rot * glm::vec3(0.0f, 0.0f, _scale.z);
    glm::mat3 mat(xAxis, yAxis, zAxis);
    glm::mat3 transInvMat = glm::inverse(glm::transpose(mat));
    return transInvMat * rhs;
}

// faster, but does not handle non-uniform scale correctly.
glm::vec3 AnimPose::xformVectorFast(const glm::vec3& rhs) const {
    return _rot * (_scale * rhs);
}

AnimPose AnimPose::operator*(const AnimPose& rhs) const {
    glm::mat4 result;
    glm_mat4u_mul(*this, rhs, result);
    return AnimPose(result);
}

AnimPose AnimPose::inverse() const {
    return AnimPose(glm::inverse(static_cast<glm::mat4>(*this)));
}

// mirror about x-axis without applying negative scale.
AnimPose AnimPose::mirror() const {
    return AnimPose(_scale, glm::quat(_rot.w, _rot.x, -_rot.y, -_rot.z), glm::vec3(-_trans.x, _trans.y, _trans.z));
}

bool AnimPose::fuzzyEqual(const AnimPose& rhs) const {
    const float SCALE_EPSILON = 0.00001f;
    const float ROT_EPSILON = 0.00001f;
    const float TRANS_EPSILON = 0.001f;
    if ((fabsf(rhs._scale.x - _scale.x) < SCALE_EPSILON) &&
        (fabsf(rhs._scale.y - _scale.y) < SCALE_EPSILON) &&
        (fabsf(rhs._scale.z - _scale.z) < SCALE_EPSILON)) {
        if ((fabsf(rhs._rot.x - _rot.x) < ROT_EPSILON) &&
            (fabsf(rhs._rot.y - _rot.y) < ROT_EPSILON) &&
            (fabsf(rhs._rot.z - _rot.z) < ROT_EPSILON) &&
            (fabsf(rhs._rot.w - _rot.w) < ROT_EPSILON)) {
            if ((fabsf(rhs._trans.x - _trans.x) < TRANS_EPSILON) &&
                (fabsf(rhs._trans.y - _trans.y) < TRANS_EPSILON) &&
                (fabsf(rhs._trans.z - _trans.z) < TRANS_EPSILON)) {
                return true;
            }
        }
    }
    return false;
}

AnimPose::operator glm::mat4() const {
    glm::vec3 xAxis = _rot * glm::vec3(_scale.x, 0.0f, 0.0f);
    glm::vec3 yAxis = _rot * glm::vec3(0.0f, _scale.y, 0.0f);
    glm::vec3 zAxis = _rot * glm::vec3(0.0f, 0.0f, _scale.z);
    return glm::mat4(glm::vec4(xAxis, 0.0f), glm::vec4(yAxis, 0.0f),
        glm::vec4(zAxis, 0.0f), glm::vec4(_trans, 1.0f));
}


