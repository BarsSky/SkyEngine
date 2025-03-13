//
// Created by f_f on 04.12.24.
//
#include <SkyEngine/vk_sky_pipelineobject.hpp>
#include <SkyEngine/vk_sky_model.hpp>
#include <SkyEngine/vk_sky_device.hpp>

pipelineObject::pipelineObject(ePipelineObjectType type, std::string object_path) {
    switch (type) {
        case ePipelineObjectType::OBJECT_3D:
            object = create_Model_3d(object_path);
            break;
        case ePipelineObjectType::OBJECT_2D:
            object = create_Model_2D();
            break;
        case ePipelineObjectType::LINE:
            object = create_Line();
            break;
        case ePipelineObjectType::GLTF:
            object = create_GLTF_Model(object_path);
            break;
        case ePipelineObjectType::GLTF_ANIMATE:
            object = create_GLTF_Model_Animate(object_path);
            break;
        case ePipelineObjectType::GLTF_SkyBox:
            object = create_GLTF_SkyBox(object_path);
            break;
        case ePipelineObjectType::TRANSPARENT_OBJECT_3D:
            object = create_Transparent_3d(object_path);
            break;
            // case ePipelineObjectType::TRANSPARENT_OBJECT_2D:
            //     break;
        case ePipelineObjectType::TERRIAN_OBJECT:
            object = create_Terrian(object_path);
            break;
        case ePipelineObjectType::PARTICLE_CPU_OBJECT:
            object = create_Particle_cpu();
            break;
        case ePipelineObjectType::PARTICLE_GPU_OBJECT:
            object = create_Particle_gpu();
            break;

        default:
            object = create_Model_3d(object_path);
    }
}

pipelineObject::pipelineObject(void *obj) {
    object = obj;
}

void *pipelineObject::create_Model_3d() {
    return object = reinterpret_cast<void *>(new Model3D());
}

void *pipelineObject::create_Model_3d(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
    return object = reinterpret_cast<void *>(new Model3D(vertices, indices));
}

void *pipelineObject::create_Model_3d(std::string object_path) {
    return reinterpret_cast<void *>(new Model(object_path, model_type::FROMOBJ));
}

void *pipelineObject::create_Particle_cpu() {
    return object = reinterpret_cast<void *>(new Partical_Model_CPU());
}

pipelineObject::pipelineObject(ePipelineObjectType type, const std::vector<Vertex> vertices,
                               std::vector<uint32_t> indices) {
    switch (type) {
        case ePipelineObjectType::OBJECT_3D:
            object = create_Model_3d(vertices, indices);
            break;
        case ePipelineObjectType::OBJECT_2D:
            object = create_Model_2D(vertices, indices);
            break;
        case ePipelineObjectType::LINE:
            object = create_Line(vertices);
            break;
        default:
            break;
    }
}

void *pipelineObject::create_Terrian(std::string object_path) {
    return object = reinterpret_cast<void *>(new Terrian_Model(object_path, model_type::FROMKTX));
}

void *pipelineObject::create_Transparent_3d(std::string object_path) {
    return object = reinterpret_cast<void *>(new Transparent_Model(object_path));
}

void *pipelineObject::create_GLTF_Model(std::string object_path) {
    return object = reinterpret_cast<void *>(new GLTF_Model(object_path));
}

void *pipelineObject::create_GLTF_Model_Animate(std::string object_path) {
    return object = reinterpret_cast<void *>(new GLTF_Model_Animate(object_path));
}

void *pipelineObject::create_GLTF_SkyBox(std::string object_path) {
    return object = reinterpret_cast<void *>(new GLTF_SkyBox(object_path));
}

void *pipelineObject::create_Model_2D() {
    return object = reinterpret_cast<void *>(new Model2D());
}

void *pipelineObject::create_Model_2D(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
    return object = reinterpret_cast<void *>(new Model2D(vertices, indices));
}

void *pipelineObject::create_Line() {
    return object = reinterpret_cast<void *>(new Line());
}

void *pipelineObject::create_Line(std::vector<Vertex> vertices) {
    return object = reinterpret_cast<void *>(new Line(vertices));
}

void *pipelineObject::create_Particle_gpu() {
    return object = reinterpret_cast<void *>(new Partical_Model_GPU());
}
