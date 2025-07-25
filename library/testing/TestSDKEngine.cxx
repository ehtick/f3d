#include <engine.h>
#include <interactor.h>
#include <log.h>
#include <options.h>
#include <scene.h>
#include <window.h>

#include <iostream>

int TestSDKEngine(int argc, char* argv[])
{
  f3d::log::setVerboseLevel(f3d::log::VerboseLevel::DEBUG);

  // clang-format off
  // Load all plugins, built or not, already loaded or not
  try { f3d::engine::loadPlugin("native", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("alembic", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("assimp", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("draco", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("exodus", {argv[3]}); } catch (...) {} // F3D_DEPRECATED
  try { f3d::engine::loadPlugin("hdf", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("occt", {argv[3]}); } catch (...) {}
  try { f3d::engine::loadPlugin("usd", {argv[3]}); } catch (...) {}
  // clang-format on

  // Test different flags combinations that makes sense
  f3d::engine eng0 = f3d::engine::createNone();
  const f3d::scene& sce = eng0.getScene();

  f3d::engine eng1 = f3d::engine::create();
  const f3d::scene& sce1 = eng1.getScene();
  const f3d::window& window1 = eng1.getWindow();
  const f3d::interactor& inter1 = eng1.getInteractor();

  f3d::engine eng2 = f3d::engine::create(true);
  const f3d::scene& sce2 = eng2.getScene();
  const f3d::window& window2 = eng2.getWindow();
  const f3d::interactor& inter2 = eng2.getInteractor();

  // Test option setters
  f3d::options opt;
  opt.model.scivis.cells = true;

  eng0.setOptions(opt);
  if (!eng0.getOptions().model.scivis.cells)
  {
    std::cerr << "Unexpected options value using f3d::engine::setOptions(const options& opt)\n";
    return EXIT_FAILURE;
  }

  opt.render.line_width = 1.7;
  eng0.setOptions(std::move(opt));
  if (eng0.getOptions().render.line_width != 1.7)
  {
    std::cerr << "Unexpected options value using f3d::engine::setOptions(options&& opt)\n";
    return EXIT_FAILURE;
  }

  // Test static information methods
  auto libInfo = f3d::engine::getLibInfo();
  if (libInfo.License != "BSD-3-Clause")
  {
    std::cerr << "Unexpected libInfo output\n";
    return EXIT_FAILURE;
  }

  auto readersInfo = f3d::engine::getReadersInfo();
  if (readersInfo.size() == 0)
  {
    std::cerr << "Unexpected readersInfo output\n";
    return EXIT_FAILURE;
  }

  // coverage for getPluginList
  std::ignore = f3d::engine::getPluginsList(std::string(argv[1]) + "configs");
  std::ignore = f3d::engine::getPluginsList("inexistent");

  return EXIT_SUCCESS;
}
