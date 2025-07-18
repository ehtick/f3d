#include <engine.h>
#include <export.h>
#include <log.h>
#include <options.h>
#include <scene.h>
#include <window.h>

#include "TestSDKHelpers.h"

#include <random>

int TestSDKDynamicHDRI(int argc, char* argv[])
{
  f3d::log::setVerboseLevel(f3d::log::VerboseLevel::INFO);

  f3d::engine eng = f3d::engine::create(true);

  f3d::scene& sce = eng.getScene();
  f3d::window& win = eng.getWindow();
  f3d::options& opt = eng.getOptions();
  win.setSize(300, 300);
  opt.ui.filename = true;
  opt.ui.filename_info = "(1/1) cow.vtp";

  sce.add(std::string(argv[1]) + "/data/cow.vtp");

  bool ret = win.render();
  if (!ret)
  {
    std::cerr << "First render failed\n";
    return EXIT_FAILURE;
  }

  // Generate a random cache path to avoid reusing any existing cache
  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> dist(1, 100000);
  std::string cachePath = std::string(argv[2]) + "/cache_" + std::to_string(dist(e1));
  eng.setCachePath(cachePath);

  // Enable HDRI ambient and skybox and check the default HDRI
  opt.render.hdri.ambient = true;
  opt.render.background.skybox = true;
  ret = TestSDKHelpers::RenderTest(eng.getWindow(), std::string(argv[1]) + "baselines/",
    std::string(argv[2]), "TestSDKDynamicHDRIDefault");
  if (!ret)
  {
    std::cerr << "Render with Default HDRI failed\n";
    return EXIT_FAILURE;
  }

  // Change the hdri and make sure it is taken into account
  opt.render.hdri.file = std::string(argv[1]) + "data/palermo_park_1k.hdr";
  ret = TestSDKHelpers::RenderTest(eng.getWindow(), std::string(argv[1]) + "baselines/",
    std::string(argv[2]), "TestSDKDynamicHDRI");
  if (!ret)
  {
    std::cerr << "Render with HDRI failed\n";
    return EXIT_FAILURE;
  }

  // Check caching is working
  std::ifstream lutFile(cachePath + "/lut.vti");
  if (!lutFile.is_open())
  {
    std::cerr << "LUT cache file not found\n";
    return EXIT_FAILURE;
  }

  // Force a cache path change to force a LUT reconfiguration and test dynamic cache path
  eng.setCachePath(std::string(argv[2]) + "/cache_" + std::to_string(dist(e1)));
  ret = TestSDKHelpers::RenderTest(eng.getWindow(), std::string(argv[1]) + "baselines/",
    std::string(argv[2]), "TestSDKDynamicHDRI");
  if (!ret)
  {
    std::cerr << "Render with HDRI with another cache path failed\n";
    return EXIT_FAILURE;
  }

  // Use an existing cache
  eng.setCachePath(cachePath);
  ret = TestSDKHelpers::RenderTest(eng.getWindow(), std::string(argv[1]) + "baselines/",
    std::string(argv[2]), "TestSDKDynamicHDRI");
  if (!ret)
  {
    std::cerr << "Render with HDRI with existing cache path failed\n";
    return EXIT_FAILURE;
  }

#if F3D_MODULE_EXR
  // Change the hdri and make sure it is taken into account
  opt.render.hdri.file = std::string(argv[1]) + "/data/kloofendal_43d_clear_1k.exr";
  ret = TestSDKHelpers::RenderTest(eng.getWindow(), std::string(argv[1]) + "baselines/",
    std::string(argv[2]), "TestSDKDynamicHDRIExr");

  if (!ret)
  {
    std::cerr << "Render with EXR HDRI failed\n";
    return EXIT_FAILURE;
  }
#endif

  return EXIT_SUCCESS;
}
