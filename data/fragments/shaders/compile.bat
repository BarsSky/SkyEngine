glslc shader.vert -o comp/shader.vert.spv
glslc shader.frag -o comp/shader.frag.spv
glslc earth.vert -o comp/earth.vert.spv
glslc earth.frag -o comp/earth.frag.spv
glslc clouds.vert -o comp/clouds.vert.spv
glslc clouds.frag -o comp/clouds.frag.spv
glslc moon.vert -o comp/moon.vert.spv
glslc moon.frag -o comp/moon.frag.spv
glslc sun.vert -o comp/sun.vert.spv
glslc sun.frag -o comp/sun.frag.spv
@rem
glslc uioverlay.frag -o comp/uioverlay.frag.spv
glslc uioverlay.vert -o comp/uioverlay.vert.spv
@rem
glslc particlefire/particle.frag -o comp/particle.frag.spv
glslc particlefire/particle.vert -o comp/particle.vert.spv
@rem
glslc terrian/terrain.vert -o comp/terrain.vert.spv
glslc terrian/terrain.frag -o comp/terrain.frag.spv
glslc terrian/terrain.tesc -o comp/terrain.tesc.spv
glslc terrian/terrain.tese -o comp/terrain.tese.spv
@rem skin
glslc gltfskinning/skinnedmodel.frag -o comp/skinnedmodel.frag.spv
glslc gltfskinning/skinnedmodel.vert -o comp/skinnedmodel.vert.spv
@rem skybox
glslc skybox/skybox.frag -o comp/skybox.frag.spv
glslc skybox/skybox.vert -o comp/skybox.vert.spv
@rem gltf scene rendering
glslc gltfscenerendering/scene.frag -o comp/scene.frag.spv
glslc gltfscenerendering/scene.vert -o comp/scene.vert.spv
@rem
glslc mesh/mesh.frag -o comp/mesh.frag.spv
glslc mesh/mesh.vert -o comp/mesh.vert.spv
@rem 2D model
glslc model2d/model2d.frag -o comp/model2d.frag.spv
glslc model2d/model2d.vert -o comp/model2d.vert.spv
@rem tools
glslc shader.vert -o comp/tools_shader.vert.spv
glslc shader.frag -o comp/tools_shader.frag.spv
@rem Axis 3D
glslc axis/axis.frag -o comp/axis.frag.spv
glslc axis/axis.vert -o comp/axis.vert.spv

@rem line
glslc line/line.frag -o comp/line.frag.spv
glslc line/line.vert -o comp/line.vert.spv
glslc line/line.geom -o comp/line.geom.spv
@rem text
glslc text/text.vert -o comp/text.vert.spv
glslc text/text.frag -o comp/text.frag.spv

@rem compute particle
glslc computeparticle/particle.frag -o comp/compute_particle.frag.spv
glslc computeparticle/particle.vert -o comp/compute_particle.vert.spv
glslc computeparticle/particle.comp -o comp/compute_particle.comp.spv
@rem shape form
glslc shape/shape.vert -o comp/shape.vert.spv
glslc shape/shape.frag -o comp/shape.frag.spv

pause
