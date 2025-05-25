#!/bin/sh
# Don't forget add access use chmod +x "file_name.sh"
# Allow also for glslc binary
# If file chaged in Windows OS first delete all \r: sed -i 's/\r//g' "file_name.sh"
#../../../tools/glslc/bin/
VK_VULKAN_BIN="../../glslc/bin/"
echo $VK_VULKAN_BIN
# create dir
mkdir ./comp
$VK_VULKAN_BIN/glslc shader.vert -o comp/shader.vert.spv
$VK_VULKAN_BIN/glslc shader.frag -o comp/shader.frag.spv
$VK_VULKAN_BIN/glslc earth.vert -o comp/earth.vert.spv
$VK_VULKAN_BIN/glslc earth.frag -o comp/earth.frag.spv
$VK_VULKAN_BIN/glslc clouds.vert -o comp/clouds.vert.spv
$VK_VULKAN_BIN/glslc clouds.frag -o comp/clouds.frag.spv
$VK_VULKAN_BIN/glslc moon.vert -o comp/moon.vert.spv
$VK_VULKAN_BIN/glslc moon.frag -o comp/moon.frag.spv
$VK_VULKAN_BIN/glslc sun.vert -o comp/sun.vert.spv
$VK_VULKAN_BIN/glslc sun.frag -o comp/sun.frag.spv
 # uioverlay
$VK_VULKAN_BIN/glslc uioverlay.frag -o comp/uioverlay.frag.spv
$VK_VULKAN_BIN/glslc uioverlay.vert -o comp/uioverlay.vert.spv
 # particle fire
 $VK_VULKAN_BIN/glslc particlefire/particle.frag -o comp/particle.frag.spv
 $VK_VULKAN_BIN/glslc particlefire/particle.vert -o comp/particle.vert.spv
 # terrian
 $VK_VULKAN_BIN/glslc terrian/terrain.vert -o comp/terrain.vert.spv
 $VK_VULKAN_BIN/glslc terrian/terrain.frag -o comp/terrain.frag.spv
 $VK_VULKAN_BIN/glslc terrian/terrain.tesc -o comp/terrain.tesc.spv
 $VK_VULKAN_BIN/glslc terrian/terrain.tese -o comp/terrain.tese.spv
 # skin
$VK_VULKAN_BIN/glslc gltfskinning/skinnedmodel.frag -o comp/skinnedmodel.frag.spv
$VK_VULKAN_BIN/glslc gltfskinning/skinnedmodel.vert -o comp/skinnedmodel.vert.spv
 # skybox
 $VK_VULKAN_BIN/glslc skybox/skybox.frag -o comp/skybox.frag.spv
 $VK_VULKAN_BIN/glslc skybox/skybox.vert -o comp/skybox.vert.spv
 # gltf scene rendering
 $VK_VULKAN_BIN/glslc gltfscenerendering/scene.frag -o comp/scene.frag.spv
 $VK_VULKAN_BIN/glslc gltfscenerendering/scene.vert -o comp/scene.vert.spv
 #
 $VK_VULKAN_BIN/glslc mesh/mesh.frag -o comp/mesh.frag.spv
 $VK_VULKAN_BIN/glslc mesh/mesh.vert -o comp/mesh.vert.spv
 # Model 2D
 $VK_VULKAN_BIN/glslc model2d/model2d.frag -o comp/model2d.frag.spv
 $VK_VULKAN_BIN/glslc model2d/model2d.vert -o comp/model2d.vert.spv
 $VK_VULKAN_BIN/glslc model2d/model2d.geom -o comp/model2d.geom.spv
  # Axis 3D
$VK_VULKAN_BIN/glslc axis/axis.frag -o comp/axis.frag.spv
$VK_VULKAN_BIN/glslc axis/axis.vert -o comp/axis.vert.spv
# compute particle
$VK_VULKAN_BIN/glslc computeparticle/particle.frag -o comp/compute_particle.frag.spv
$VK_VULKAN_BIN/glslc computeparticle/particle.vert -o comp/compute_particle.vert.spv
$VK_VULKAN_BIN/glslc computeparticle/particle.comp -o comp/compute_particle.comp.spv
# line
$VK_VULKAN_BIN/glslc line/line.frag -o comp/line.frag.spv
$VK_VULKAN_BIN/glslc line/line.vert -o comp/line.vert.spv
$VK_VULKAN_BIN/glslc line/line.geom -o comp/line.geom.spv
# text
$VK_VULKAN_BIN/glslc text/text.vert -o comp/text.vert.spv
$VK_VULKAN_BIN/glslc text/text.frag -o comp/text.frag.spv
# text plate
$VK_VULKAN_BIN/glslc plate_text/plate_text.vert -o comp/plate_text.vert.spv
$VK_VULKAN_BIN/glslc plate_text/plate_text.frag -o comp/plate_text.frag.spv
# shape form
$VK_VULKAN_BIN/glslc shape/shape.vert -o comp/shape.vert.spv
$VK_VULKAN_BIN/glslc shape/shape.frag -o comp/shape.frag.spv
echo "Press [Enter] key to continue..."
read
cp -r ./comp/. ../../shaders
echo "Done"