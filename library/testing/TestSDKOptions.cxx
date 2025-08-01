#include "PseudoUnitTest.h"
#include "TestSDKHelpers.h"

#include <export.h>
#include <options.h>
#include <types.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

int TestSDKOptions(int argc, char* argv[])
{
  PseudoUnitTest test;

  f3d::options opt;

  // Test bool
  opt.setAsString("model.scivis.cells", "false");
  test("setAsString bool", opt.getAsString("model.scivis.cells") == "false");

  opt.setAsString("model.scivis.cells", "1");
  test("setAsString bool", opt.getAsString("model.scivis.cells") == "true");

  opt.model.scivis.cells = false;
  test("getAsString bool", opt.getAsString("model.scivis.cells") == "false");

  opt.set("model.scivis.cells", true);
  test("set/get bool", std::get<bool>(opt.get("model.scivis.cells")) == true);

  opt.set("model.scivis.cells", false);
  opt.toggle("model.scivis.cells");
  test("toggle", opt.getAsString("model.scivis.cells") == "true");
  opt.toggle("render.show_edges");
  test("toggle optional", opt.getAsString("render.show_edges") == "true");

  // Test int
  opt.setAsString("model.scivis.component", "2");
  test("setAsString int", opt.getAsString("model.scivis.component") == "2");

  opt.model.scivis.component = 3;
  test("getAsString int", opt.getAsString("model.scivis.component") == "3");

  opt.set("model.scivis.component", 1);
  test("set/get int", std::get<int>(opt.get("model.scivis.component")) == 1);

  // Test double
  opt.setAsString("render.line_width", "2.14");
  test("setAsString double", opt.getAsString("render.line_width") == "2.14");

  opt.render.line_width = 2.13;
  test("getAsString double", opt.getAsString("render.line_width") == "2.13");

  opt.set("render.line_width", 1.7);
  test("set/get double", std::get<double>(opt.get("render.line_width")) == 1.7);

  // Test ratio_t
  opt.setAsString("scene.animation.speed_factor", "3.17");
  test("setAsString ratio_t", opt.getAsString("scene.animation.speed_factor") == "3.17");

  opt.scene.animation.speed_factor = f3d::ratio_t(3.18);
  test("getAsString ratio_t", opt.getAsString("scene.animation.speed_factor") == "3.18");

  opt.set("scene.animation.speed_factor", 3.17);
  test("set/get ratio_t", std::get<double>(opt.get("scene.animation.speed_factor")) == 3.17);

  // Test string
  opt.setAsString("render.effect.final_shader", "testAsString");
  test("setAsString string", opt.getAsString("render.effect.final_shader") == "testAsString");

  opt.render.effect.final_shader = "testInStruct";
  test("getAsString string", opt.getAsString("render.effect.final_shader") == "testInStruct");

  std::string inputString = "test";
  opt.set("render.effect.final_shader", inputString);
  test("set/get string", std::get<std::string>(opt.get("render.effect.final_shader")) == "test");

  // Test path
  opt.setAsString("render.hdri.file", "/path/to/test/string");
  test("setAsString path", opt.getAsString("render.hdri.file") == "/path/to/test/string");

  opt.setAsString("render.hdri.file", "/path/not/valid/../../to/test/string");
  test("setAsString collapse path", opt.getAsString("render.hdri.file") == "/path/to/test/string");

  opt.render.hdri.file = "/path/to/test/struct";
  test("getAsString path", opt.getAsString("render.hdri.file") == "/path/to/test/struct");

  std::string inputPath = "/path/to/test/variant";
  opt.set("render.hdri.file", inputPath);
  test(
    "set/get path", std::get<std::string>(opt.get("render.hdri.file")) == "/path/to/test/variant");

  // Test int vector
  opt.setAsString("scene.animation.indices", "1,2");
  test("setAsString vector<int>", opt.getAsString("scene.animation.indices") == "1,2");

  opt.setAsString("scene.animation.indices", "3, 7");
  test("setAsString spaces vector<int>", opt.getAsString("scene.animation.indices") == "3,7");

  opt.scene.animation.indices = { 4, 12 };
  test("getAsString vector<int>", opt.getAsString("scene.animation.indices") == "4,12");

  opt.set("scene.animation.indices", std::vector<int>{ 7, 3, 1 });
  test("set/get vector<int>",
    std::get<std::vector<int>>(opt.get("scene.animation.indices")) == std::vector<int>{ 7, 3, 1 });

  // Test double vector
  opt.setAsString("model.scivis.range", "0.7,1.4");
  test("setAsString vector<double>", opt.getAsString("model.scivis.range") == "0.7,1.4");

  opt.setAsString("model.scivis.range", "0.8, 1.5");
  test("setAsString spaces vector<double>", opt.getAsString("model.scivis.range") == "0.8,1.5");

  opt.model.scivis.range = { 0.9, 1.2 };
  test("getAsString vector<double>", opt.getAsString("model.scivis.range") == "0.9,1.2");

  opt.set("model.scivis.range", std::vector<double>{ 0.5, 1.5 });
  test("set/get vector<double>",
    std::get<std::vector<double>>(opt.get("model.scivis.range")) ==
      std::vector<double>{ 0.5, 1.5 });

  // Test color_t
  opt.setAsString("render.background.color", "0.1,0.2,0.4");
  test("setAsString color", opt.getAsString("render.background.color") == "0.1,0.2,0.4");

  opt.setAsString("render.background.color", "0.1, 0.3, 0.4");
  test("setAsString spaces color", opt.getAsString("render.background.color") == "0.1,0.3,0.4");

  opt.render.background.color = { 0.1, 0.2, 0.5 };
  test("getAsString color", opt.getAsString("render.background.color") == "0.1,0.2,0.5");

  opt.set("render.background.color", f3d::color_t{ 0.1, 0.2, 0.3 });
  test("set/get color",
    std::get<std::vector<double>>(opt.get("render.background.color")) ==
      std::vector<double>{ 0.1, 0.2, 0.3 });

  test.expect<f3d::type_construction_exception>(
    "color_t invalid size vector", [&]() { f3d::color_t(std::vector<double>{ 0.1, 0.2 }); });
  test.expect<f3d::type_construction_exception>(
    "color_t invalid size list", [&]() { f3d::color_t({ 0.1, 0.2, 0.3, 0.4 }); });

  f3d::color_t color({ 0.1, 0.2, 0.3 });
  color[0] = 0.7;
  const f3d::color_t* colorPtr = &color;
  test("color operator[]", color[0] == 0.7);
  test("color operator[] const", (*colorPtr)[0] == 0.7);
  test.expect<f3d::type_access_exception>(
    "color_t invalid access", [&]() { double val = color[3]; });
  test.expect<f3d::type_access_exception>(
    "color_t const invalid access", [&]() { double val = (*colorPtr)[3]; });
  test("color red", color.r() == 0.7);
  test("color green", color.g() == 0.2);
  test("color blue", color.b() == 0.3);

  std::stringstream ss;
  ss << f3d::color_t(0.1, 0.2, 0.3);
  test("color to string", ss.str() == "0.1,0.2,0.3");

  test.expect<f3d::options::parsing_exception>("setAsString color with incorrect size",
    [&]() { opt.setAsString("render.background.color", "0.1,0.2,0.3,0.4"); });

  test.expect<f3d::options::incompatible_exception>("set color with incorrect size",
    [&]() { opt.set("render.background.color", std::vector<double>{ 0.1, 0.2 }); });

  // Test direction_t (rely on testing from color_t)
  opt.setAsString("scene.up_direction", "+X");
  test("setAsString direction", opt.getAsString("scene.up_direction"), "+X");

  f3d::direction_t dir({ 0.707, -0.707, 0 });
  test("direction x", dir.x() == 0.707);
  test("direction y", dir.y() == -0.707);
  test("direction z", dir.z() == 0);

  std::stringstream ssDir;
  ssDir << f3d::direction_t(0, 0, -1.0);
  test("direction to string", ssDir.str(), "-Z");

  test.expect<f3d::options::parsing_exception>("setAsString direction with incorrect size",
    [&]() { opt.setAsString("scene.up_direction", "0.1,0.2,0.3,0.4"); });

  // Test colormap_t
  opt.setAsString("model.scivis.colormap", "0,0,0,0,1,1,1,1");
  test("setAsString colormap", opt.getAsString("model.scivis.colormap"), "0,#000000,1,#ffffff");

  opt.setAsString("model.scivis.colormap", "0,0,  0,0,  1,0,  1,1");
  test("setAsString spaces colormap",
    opt.getAsString("model.scivis.colormap") == "0,#000000,1,#00ffff");

  opt.model.scivis.colormap = { 0, 0, 0, 0, 1, 1, 0, 1 };
  test("getAsString colormap", opt.getAsString("model.scivis.colormap") == "0,#000000,1,#ff00ff");

  opt.set("model.scivis.colormap", std::vector<double>{ 0, 0, 0, 0, 1, 1, 1, 0 });
  test("set/get colormap",
    std::get<std::vector<double>>(opt.get("model.scivis.colormap")) ==
      std::vector<double>{ 0, 0, 0, 0, 1, 1, 1, 0 });

  // Test transform2d_t
  opt.setAsString("model.textures_transform", "1,0,0,0,-1,0,0,0,1");
  test("setAsString vector transform2d", opt.getAsString("model.textures_transform"),
    "1,0,0,0,-1,0,0,0,1");

  opt.setAsString("model.textures_transform", "scale:0.1");
  test("setAsString scale transform2d", opt.getAsString("model.textures_transform"),
    "0.1,0,0,0,0.1,0,0,0,1");

  opt.setAsString("model.textures_transform", "translation:0.51,2.1");
  test("setAsString translation transform2d", opt.getAsString("model.textures_transform"),
    "1,0,0.51,0,1,2.1,0,0,1");

  opt.model.textures_transform = { 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5 };
  test("getAsString transform2d",
    opt.getAsString("model.textures_transform") == "0.5,0,0,0,0.5,0,0,0,0.5");

  opt.setAsString("model.textures_transform", "angle:90.0");
  test("setAsString/get angle transform2d",
    std::get<std::vector<double>>(opt.get("model.textures_transform")) ==
      std::vector<double>{ std::cos(TestSDKHelpers::Degrees2Radians(90.0)),
        -std::sin(TestSDKHelpers::Degrees2Radians(90.0)), 0,
        std::sin(TestSDKHelpers::Degrees2Radians(90.0)),
        std::cos(TestSDKHelpers::Degrees2Radians(90.0)), 0, 0, 0, 1 });

  opt.setAsString("model.textures_transform", "scale:0.1;translation:0.51,2.1;angle:60.0");
  test("setAsString/get scale/translation/angle transform2d",
    std::get<std::vector<double>>(opt.get("model.textures_transform")) ==
      std::vector<double>{ 0.1 * std::cos(TestSDKHelpers::Degrees2Radians(60.0)),
        0.1 * -std::sin(TestSDKHelpers::Degrees2Radians(60.0)), 0.51,
        0.1 * std::sin(TestSDKHelpers::Degrees2Radians(60.0)),
        0.1 * std::cos(TestSDKHelpers::Degrees2Radians(60.0)), 2.1, 0, 0, 1 });

  f3d::transform2d_t nanTransform =
    f3d::transform2d_t(f3d::double_array_t<2>(std::vector<double>(2, 1)),
      f3d::double_array_t<2>(std::vector<double>(2, 0)), std::numeric_limits<double>::quiet_NaN());
  opt.model.textures_transform = nanTransform;
  test("NaN angle transform2d", opt.getAsString("model.textures_transform") == "1,0,0,0,1,0,0,0,1");

  nanTransform = f3d::transform2d_t(
    f3d::double_array_t<2>(std::vector<double>(2, std::numeric_limits<double>::quiet_NaN())),
    f3d::double_array_t<2>(std::vector<double>(2, std::numeric_limits<double>::quiet_NaN())),
    std::numeric_limits<double>::quiet_NaN());
  opt.model.textures_transform = nanTransform;
  test(
    "NaN values transform2d", opt.getAsString("model.textures_transform") == "1,0,0,0,1,0,0,0,1");

  // Test closest option
  auto closest = opt.getClosestOption("modle.sciivs.cell");
  test("closest option", closest.first == "model.scivis.cells" && closest.second == 5);

  closest = opt.getClosestOption("model.scivis.cells");
  test("closest option exact", closest.first == "model.scivis.cells" && closest.second == 0);

  // Test chaining options
  opt.setAsString("model.scivis.cells", "false").setAsString("model.scivis.cells", "true");
  test("chaining setAsString calls", opt.getAsString("model.scivis.cells") == "true");

  opt.set("model.scivis.cells", true).set("model.scivis.cells", false);
  test("chaining set calls", std::get<bool>(opt.get("model.scivis.cells")) == false);

  // Test toggle error paths
  test.expect<f3d::options::incompatible_exception>(
    "toggle non-bool", [&]() { opt.toggle("render.line_width"); });

  // Test parsing error path
  test.expect<f3d::options::parsing_exception>(
    "set invalid int string", [&]() { opt.setAsString("model.scivis.component", "invalid"); });

  test.expect<f3d::options::parsing_exception>("set out-of-range int string",
    [&]() { opt.setAsString("model.scivis.component", "2147483648"); });

  test.expect<f3d::options::parsing_exception>(
    "set invalid double string", [&]() { opt.setAsString("render.line_width", "invalid"); });

  test.expect<f3d::options::parsing_exception>("set out-of-range double string", [&]() {
    opt.setAsString("render.line_width",
      "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
      "34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234"
      "56789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456"
      "78901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
      "90123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
      "1234567890123456789012345678901234567890");
  });

  // Test copy operator and constructor
  opt.render.background.color = { 0.1, 0.2, 0.7 };
  opt.render.line_width = 2.17;
  f3d::options opt2 = opt;
  test("copy constructor", opt2.render.line_width == 2.17);

  f3d::options opt3;
  opt3 = opt2;
  test("copy operator", opt3.render.line_width == 2.17);

  f3d::options opt4 = std::move(opt3);
  test("move constructor", opt4.render.line_width == 2.17);

  f3d::options opt5;
  opt5 = std::move(opt4);
  test("move operator", opt5.render.line_width == 2.17);

  // Test getNames
  std::vector<std::string> names = f3d::options::getAllNames();
  test(
    "getAllNames find", std::find(names.begin(), names.end(), "render.point_size") != names.end());

  std::vector<std::string> setNames = opt.getNames();
  test("getNames count", setNames.size() != 0);

  // Test isSame/copy/hasValue
  test("isSame", opt.isSame(opt2, "render.line_width"));
  test("isSame unset", opt.isSame(opt2, "render.point_size"));

  test("hasValue", opt.hasValue("render.line_width"));
  test("hasValue", !opt.hasValue("render.point_size"));

  opt2.render.line_width = 3.12;
  test("not isSame", !opt.isSame(opt2, "render.line_width"));

  opt2.copy(opt, "render.line_width");
  test("copy", opt2.render.line_width == 2.17);

  // Test isSame/copy vector
  test("isSame with vectors", opt.isSame(opt2, "render.background.color"));

  opt2.render.background.color = { 0.1, 0.2, 0.6 };
  test("not isSame with vectors", !opt.isSame(opt2, "render.background.color"));

  opt2.copy(opt, "render.background.color");
  test("copy with vectors", opt2.render.background.color == f3d::color_t({ 0.1, 0.2, 0.7 }));

  // Test isSame/copy error path
  test.expect<f3d::options::inexistent_exception>(
    "inexistent_exception exception on isSame", [&]() { std::ignore = opt.isSame(opt2, "dummy"); });

  test.expect<f3d::options::inexistent_exception>(
    "inexistent_exception exception on copy", [&]() { opt.copy(opt2, "dummy"); });

  // Test set/get error paths
  test.expect<f3d::options::incompatible_exception>(
    "incompatible_exception exception on set", [&]() { opt.set("model.scivis.cells", 2.13); });

  test.expect<f3d::options::inexistent_exception>(
    "inexistent_exception exception on set", [&]() { opt.set("dummy", 2.13); });

  test.expect<f3d::options::inexistent_exception>(
    "inexistent_exception exception on get", [&]() { std::ignore = opt.get("dummy"); });

  test.expect<f3d::options::no_value_exception>(
    "no_value_exception exception on get", [&]() { std::ignore = opt.get("render.point_size"); });

  // Test setAsString/getAsString error paths
  test.expect<f3d::options::inexistent_exception>(
    "inexistent_exception exception on setAsString", [&]() { opt.setAsString("dummy", "2.13"); });

  test.expect<f3d::options::inexistent_exception>("inexistent_exception exception on getAsString",
    [&]() { std::ignore = opt.getAsString("dummy"); });

  test.expect<f3d::options::no_value_exception>("no_value_exception exception on getAsString",
    [&]() { std::ignore = opt.getAsString("render.point_size"); });

  f3d::options opt6{};

  // Test isOptional optional values
  test("isOptional with optional value", opt6.isOptional("model.scivis.array_name"));
  test("isOptional with optional value", opt6.isOptional("model.scivis.range"));

  // Test isOptional non-optional values
  test("isOptional with non-optional value", opt6.isOptional("model.scivis.cells") == false);
  test("isOptional with non-optional value", opt6.isOptional("model.scivis.enable") == false);

  // Test isOptional non-existent options
  test.expect<f3d::options::inexistent_exception>(
    "isOptional with non-existent option", [&]() { std::ignore = opt6.isOptional("dummy"); });

  f3d::options opt7{};

  // Test reset non-optional values
  opt7.scene.up_direction = { 0, 0, 1 };
  opt7.reset("scene.up_direction");
  test("reset non-optional values", opt7.scene.up_direction == f3d::direction_t{ 0, 1, 0 });

  // Test reset optional values
  opt7.model.scivis.array_name = "dummy";
  opt7.reset("model.scivis.array_name");
  test.expect<f3d::options::no_value_exception>(
    "reset non-optional values", [&]() { std::ignore = opt7.get("model.scivis.array_name"); });

  // Test reset non-existent option
  test.expect<f3d::options::inexistent_exception>(
    "reset with non-existent option", [&]() { opt7.reset("dummy"); });

  f3d::options opt8{};

  // Test removeValue optional values
  opt8.model.scivis.array_name = "dummy";
  opt8.removeValue("model.scivis.array_name");
  test.expect<f3d::options::no_value_exception>(
    "removeValue optional values", [&]() { std::ignore = opt8.get("model.scivis.array_name"); });

  // Test removeValue non-optional values
  test.expect<f3d::options::incompatible_exception>(
    "removeValue non-optional values", [&]() { opt8.removeValue("model.scivis.cells"); });

  // Test removeValue non-optional values
  test.expect<f3d::options::inexistent_exception>(
    "removeValue non-existent option", [&]() { opt8.removeValue("dummy"); });

  return test.result();
}
