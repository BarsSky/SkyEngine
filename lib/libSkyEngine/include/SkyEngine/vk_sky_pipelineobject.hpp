//
// Created by ubuntu on 12.10.23.
//

#ifndef VKDISPLAY_PIPLINEOBJECT_H
#define VKDISPLAY_PIPLINEOBJECT_H

#include <SkyEngine/vk_sky_object.hpp>
#include <SkyEngine/vk_sky_model.hpp>
/*
 * We can add new pipelineObject to draw_objects
 * TODO: add functional:
 * remove added objects
 * or configure scenes
 */


enum class LIBSKYENGINE_EXPORT ePipelineObjectType : unsigned int {
    OBJECT_3D,
    OBJECT_2D,
    LINE,
    GLTF,
    GLTF_ANIMATE,
    GLTF_SkyBox,
    TRANSPARENT_OBJECT_3D,
    TRANSPARENT_OBJECT_2D,
    FROM_BUFFER,
    TERRIAN_OBJECT,
    PARTICLE_CPU_OBJECT,
    PARTICLE_GPU_OBJECT,
    TEXT_OVERLAY
};

/**
 *  Класс помощник для более быстрого формирования объектов
 */
// TODO: Закрыть данный класс от пользователя, реализовывать формирование через штатный открытый API класса Magick
struct LIBSKYENGINE_EXPORT pipelineObject {
    explicit pipelineObject(ePipelineObjectType type, std::vector<Vertex> vertices = std::vector<Vertex>(),
                   std::vector<uint32_t> indices = std::vector<uint32_t>());

    pipelineObject(ePipelineObjectType type, std::string object_path);

    explicit pipelineObject(void* obj);

    virtual ~pipelineObject() = default;

    void *create_Model_3d();

    void *create_Model_3d(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    static void *create_Model_3d(std::string object_path);

    // For particals
    void *create_Particle_cpu();

    void *create_Particle_gpu();

    void *create_Terrian(std::string object_path);

    void *create_Transparent_3d(std::string object_path);

    void *create_GLTF_Model(std::string object_path);

    void *create_GLTF_Model_Animate(std::string object_path);

    void *create_GLTF_SkyBox(std::string object_path);

    void *create_Model_2D();

    void *create_Model_2D(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void *create_Line();

    void *create_TextOverlay();

    void *create_Line(std::vector<Vertex> vertices);

    //
//    uniformsBuffers ubo;
    void *object;
};

#endif //VKDISPLAY_PIPLINEOBJECT_H
