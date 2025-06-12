
#include <cmath>


struct CameraVec3 {
  CameraVec3(float _x, float _y, float _z);

  ~CameraVec3() = default;

  float x, y, z;
};

CameraVec3::CameraVec3(float _x, float _y, float _z) {
  x = _x;
  y = _y;
  z = _z;
}

struct CameraQuat {
  CameraQuat(float _x, float _y, float _z, float _w) :
      x(_x),
      y(_y),
      z(_z),
      w(_w) {}

  ~CameraQuat() = default;

  float x, y, z, w;
};


static CameraVec3 cm_init_vec3(float _x, float _y, float _z) {
  return {_x, _y, _z};
}

static CameraQuat cm_init_quat(float _x, float _y, float _z, float _w) {
  return {_x, _y, _z, _w};
}

static CameraQuat cm_invert(CameraQuat _a) {
  _a.x = -_a.x;
  _a.y = -_a.y;
  _a.z = -_a.z;
  return _a;
}

static CameraQuat cm_mulQuat(CameraQuat _a, CameraQuat _b) {
  /// Adapted from bx/include/bx//math.inl#L836 function mul
  const float ax = _a.x;
  const float ay = _a.y;
  const float az = _a.z;
  const float aw = _a.w;

  const float bx = _b.x;
  const float by = _b.y;
  const float bz = _b.z;
  const float bw = _b.w;

  return cm_init_quat(
      aw * bx + ax * bw + ay * bz - az * by,
      aw * by - ax * bz + ay * bw + az * bx,
      aw * bz + ax * by - ay * bx + az * bw,
      aw * bw - ax * bx - ay * by - az * bz
  );
  /// Adaption end
}

static CameraVec3 cm_mul(CameraVec3 _v, CameraQuat _q) {
  /// Adapted from bx/include/bx//math.inl#L857 function mul
  const CameraQuat tmp0 = cm_invert(_q);
  const CameraQuat qv = cm_init_quat(_v.x, _v.y, _v.z, 0.0f);
  const CameraQuat tmp1 = cm_mulQuat(tmp0, qv);
  /// Adaption end
  const CameraQuat result = cm_mulQuat(tmp1, _q);
  return cm_init_vec3(result.x, result.y, result.z);
}

static CameraVec3 cm_add(CameraVec3 _a, CameraVec3 _b) {
  _a.x += _b.x;
  _a.y += _b.y;
  _a.z += _b.z;
  return _a;
}

static CameraVec3 cm_scale(CameraVec3 _a, float _b) {
  _a.x *= _b;
  _a.y *= _b;
  _a.z *= _b;
  return _a;
}

static CameraVec3 cm_cross(CameraVec3 _a, CameraVec3 _b) {
  /// Adapted from bx/include/bx//math.inl#L617 function cross
  return cm_init_vec3(
      _a.y * _b.z - _a.z * _b.y,
      _a.z * _b.x - _a.x * _b.z,
      _a.x * _b.y - _a.y * _b.x
  );
  /// Adaption end
}

static float cm_min(float _a, float _b) {
  return (_a < _b) ? _a : _b;
}

static float cm_max(float _a, float _b) {
  return (_a > _b) ? _a : _b;
}

static float cm_sqrt(float _a) {
  return sqrtf(_a);
}

static CameraVec3 cm_toEuler(CameraQuat _a) {
  /// Adapted from bx/include/bx//math.inl#L927 function toEuler
  const float xx = _a.x;
  const float yy = _a.y;
  const float zz = _a.z;
  const float ww = _a.w;
  const float xsq = xx * xx;
  const float ysq = yy * yy;
  const float zsq = zz * zz;

  return cm_init_vec3(
      atan2f(2.0f * (xx * ww - yy * zz), 1.0f - 2.0f * (xsq + zsq)),
      atan2f(2.0f * (yy * ww + xx * zz), 1.0f - 2.0f * (ysq + zsq)),
      asinf(2.0f * (xx * yy + zz * ww))
  );
  /// Adaption end
}

static CameraQuat cm_fromAxisAngle(CameraVec3 _axis, float _angle) {
  /// Adapted from bx/include/bx//math.inl#L996 function fromAxisAngle
  const float ha = _angle * 0.5f;
  const float sa = sinf(ha);

  return cm_init_quat(
      _axis.x * sa,
      _axis.y * sa,
      _axis.z * sa,
      cosf(ha)
  );
  /// Adaption end
}

static CameraQuat cm_normalizeQuat(CameraQuat _a) {
  /// Adapted from bx/include/bx//math.inl#L877 function normalize
  const float norm = _a.x * _a.x + _a.y * _a.y + _a.z * _a.z + _a.w * _a.w;
  if (0.0f < norm) {
    const float invNorm = powf(norm, -0.5f);

    _a.x *= invNorm;
    _a.y *= invNorm;
    _a.z *= invNorm;
    _a.w *= invNorm;
    return _a;
  }

  return cm_init_quat(0.0f, 0.0f, 0.0f, 1.0f);
  /// Adaption end
}

static CameraVec3 cm_normalizeVec3(CameraVec3 _a) {
  /// Adapted from bx/include/bx//math.inl#L663 function normalize
  const float invLen = 1.0f / sqrtf(_a.x * _a.x + _a.y * _a.y + _a.z * _a.z);
  return cm_scale(_a, invLen);
  /// Adaption end
}

static CameraVec3 cm_negate(CameraVec3 _a) {
  _a.x = -_a.x;
  _a.y = -_a.y;
  _a.z = -_a.z;
  return _a;
}

static void cm_matrixFromQuat(float *_result, CameraQuat _rotation) {
  /// Adapted from bx/include/bx//math.inl#L1142 function mtxFromQuaternion
  const float qx = _rotation.x;
  const float qy = _rotation.y;
  const float qz = _rotation.z;
  const float qw = _rotation.w;

  const float x2 = qx + qx;
  const float y2 = qy + qy;
  const float z2 = qz + qz;
  const float x2x = x2 * qx;
  const float x2y = x2 * qy;
  const float x2z = x2 * qz;
  const float x2w = x2 * qw;
  const float y2y = y2 * qy;
  const float y2z = y2 * qz;
  const float y2w = y2 * qw;
  const float z2z = z2 * qz;
  const float z2w = z2 * qw;

  _result[0] = 1.0f - (y2y + z2z);
  _result[1] = x2y - z2w;
  _result[2] = x2z + y2w;
  _result[3] = 0.0f;

  _result[4] = x2y + z2w;
  _result[5] = 1.0f - (x2x + z2z);
  _result[6] = y2z - x2w;
  _result[7] = 0.0f;

  _result[8] = x2z - y2w;
  _result[9] = y2z + x2w;
  _result[10] = 1.0f - (x2x + y2y);
  _result[11] = 0.0f;

  _result[12] = 0.0f;
  _result[13] = 0.0f;
  _result[14] = 0.0f;
  _result[15] = 1.0f;
  /// Adaption end
}
