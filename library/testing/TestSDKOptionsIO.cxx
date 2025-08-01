#include "PseudoUnitTest.h"
#include "TestSDKHelpers.h"

#include <export.h>
#include <options.h>
#include <types.h>

#include <cmath>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class ParsingTest : public PseudoUnitTest
{
public:
  template<typename T>
  void parse(const std::string& label, const std::string& input, const T& expected)
  {
    PseudoUnitTest::operator()("parse: " + label + " `" + input + "`", [&]() {
      const T actual = f3d::options::parse<T>(input);
      if (actual != expected)
      {
        throw this->comparisonMessage(actual, expected, "!=");
      }
    });
  }

  template<typename T, typename E>
  void parse_expect(const std::string& label, const std::string& input)
  {
    PseudoUnitTest::expect<E>("parse exception: " + label + " `" + input + "`",
      [&]() { std::ignore = f3d::options::parse<T>(input); });
  }

  template<typename T>
  void format(const std::string& label, const T& input, const std::string& expected)
  {
    PseudoUnitTest::operator()("format: " + label + " `" + expected + "`", [&]() {
      const std::string actual = f3d::options::format<T>(input);
      if (actual != expected)
      {
        throw this->comparisonMessage(actual, expected, "!=");
      }
    });
  }
};

int TestSDKOptionsIO(int argc, char* argv[])
{
  const std::string outOfRangeDoubleStr(
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
    "34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234"
    "56789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456"
    "78901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
    "90123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "1234567890123456789012345678901234567890");

  using parsing_exception = f3d::options::parsing_exception;
  ParsingTest test;

  test.parse<bool>("bool", "true", true);
  test.parse<bool>("bool", "false", false);
  test.parse<bool>("bool", "yes", true);
  test.parse<bool>("bool", "Yes", true);
  test.parse<bool>("bool", "no", false);
  test.parse<bool>("bool", "1", true);
  test.parse<bool>("bool", "0", false);
  test.parse_expect<bool, parsing_exception>("invalid bool", "foo");

  test.format<bool>("bool", true, "true");
  test.format<bool>("bool", false, "false");

  test.parse<int>("int", "123", 123);
  test.parse<int>("int", "-123", -123);
  test.parse<int>("int", "+123", +123);
  test.parse_expect<int, parsing_exception>("invalid int", "1.2");
  test.parse_expect<int, parsing_exception>("invalid int", "abc");
  test.parse_expect<int, parsing_exception>("invalid int",
    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
  test.format<int>("int", 123, "123");
  test.format<int>("int", -123, "-123");

  test.parse<double>("double", "123", 123.0);
  test.parse<double>("double", "-123.45", -123.45);
  test.parse<double>("double", "+1e-3", 0.001);
  test.parse_expect<double, parsing_exception>("invalid double", "1.2.3");
  test.parse_expect<double, parsing_exception>("invalid double", "abc");
  test.parse_expect<double, parsing_exception>("out of range double", outOfRangeDoubleStr);
  test.format<double>("double", 0.001, "0.001");
  test.format<double>("double", -123.45, "-123.45");

  test.parse<std::string>("std::string", "foobar", "foobar");
  test.parse<std::string>("std::string", "  foobar   ", "foobar");
  test.format<std::string>("std::string", "foobar", "foobar");
  test.format<std::string>("std::string", "  foobar  ", "  foobar  ");

  test.parse<fs::path>("fs::path", "/path/to/file.ext", "/path/to/file.ext");
  test.parse<fs::path>("fs::path", "/path/not/valid/../../to/file.ext", "/path/to/file.ext");
  test.format<fs::path>("fs::path", "/path/to/file.ext", "/path/to/file.ext");

  test.parse<f3d::ratio_t>("ratio_t", "0.1234", f3d::ratio_t(0.1234));
  test.parse<f3d::ratio_t>("ratio_t", "12.34%", f3d::ratio_t(0.1234));
  test.parse<f3d::ratio_t>("ratio_t", "1/2", f3d::ratio_t(0.5));
  test.parse<f3d::ratio_t>("ratio_t", "1:2", f3d::ratio_t(0.5));
  test.parse<f3d::ratio_t>("ratio_t", "-2/-3.5", f3d::ratio_t(2.0 / 3.5));
  test.parse_expect<f3d::ratio_t, parsing_exception>("invalid ratio_t", "12.34&");
  test.parse_expect<f3d::ratio_t, parsing_exception>("invalid ratio_t", "1/2/3");
  test.parse_expect<f3d::ratio_t, parsing_exception>("out of range ratio_t", outOfRangeDoubleStr);
  test.format<f3d::ratio_t>("ratio_t", f3d::ratio_t(.1234), "0.1234");

  test.parse<std::vector<int>>("std::vector<int>", "1, 2, 3", { 1, 2, 3 });
  test.parse<std::vector<int>>("std::vector<int>", "1,2,3", { 1, 2, 3 });
  test.format<std::vector<int>>("std::vector<int>", { 1, 2, 3 }, "1,2,3");

  test.parse<std::vector<double>>("std::vector<double>", "0.1,0.2,0.3", { 0.1, 0.2, 0.3 });
  test.parse<std::vector<double>>("std::vector<double>", "  0.1,  0.2 , 0.3 ", { 0.1, 0.2, 0.3 });
  test.format<std::vector<double>>("std::vector<double>", { 0.1, 0.2, 0.3 }, "0.1,0.2,0.3");

  test.parse<f3d::color_t>("color_t", "0.1,0.2,0.3", { 0.1, 0.2, 0.3 });
  test.parse<f3d::color_t>("color_t", "  0.1,  0.2 , 0.3 ", { 0.1, 0.2, 0.3 });
  test.parse<f3d::color_t>("color_t", "#FFFFFF", { 1.0, 1.0, 1.0 });
  test.parse<f3d::color_t>("color_t", "  #1a2B3c ",
    { static_cast<double>(26.0 / 255.0), static_cast<double>(43.0 / 255.0),
      static_cast<double>(60.0 / 255.0) });
  test.parse<f3d::color_t>("color_t", "#123",
    { static_cast<double>(17.0 / 255.0), static_cast<double>(34.0 / 255.0),
      static_cast<double>(51.0 / 255.0) });
  test.parse<f3d::color_t>("color_t", "  RGB( 255 ,  255 ,255 )  ", { 1.0, 1.0, 1.0 });
  test.parse<f3d::color_t>("color_t", "rgb(35,149,39)",
    { static_cast<double>(35.0 / 255.0), static_cast<double>(149.0 / 255.0),
      static_cast<double>(39.0 / 255.0) });
  test.parse<f3d::color_t>("color_t", "light_salmon",
    { 1.0, static_cast<double>(160.0 / 255.0), static_cast<double>(122.0 / 255.0) });
  test.parse<f3d::color_t>("color_t", "LightSalmon",
    { 1.0, static_cast<double>(160.0 / 255.0), static_cast<double>(122.0 / 255.0) });
  test.parse<f3d::color_t>("color_t", "hsv(0,0%,75%)", { 0.75, 0.75, 0.75 });
  test.parse<f3d::color_t>("color_t", " HSV( 240 , 100 % , 100 % )", { 0.0, 0.0, 1.0 });
  test.parse<f3d::color_t>("color_t", "hsl( 240 , 100 % , 50 % )", { 0.0, 0.0, 1.0 });
  test.parse<f3d::color_t>("color_t", "hwb(240,0%,0%)", { 0.0, 0.0, 1.0 });
  test.parse<f3d::color_t>("color_t", "cmyk(100,100%,0,0)", { 0.0, 0.0, 1.0 });
  test.parse_expect<f3d::color_t, parsing_exception>(
    "invalid rgb() value color_t", "rgb(300,255,255)");
  test.parse_expect<f3d::color_t, parsing_exception>(
    "invalid hsl() value color_t", "hsl(361,120,255)");
  test.parse_expect<f3d::color_t, parsing_exception>("invalid hsv() color_t", "hsv(100,120%,0)");
  test.parse_expect<f3d::color_t, parsing_exception>("invalid format color_t", "hxb(240,0%,0%)");
  test.parse_expect<f3d::color_t, parsing_exception>("invalid color_t", "cmyk(200%,12%,34%,56%)");
  test.parse_expect<f3d::color_t, parsing_exception>("incorrect size color_t", "0.1,0.2,0.3,0.4");
  test.format<f3d::color_t>("color_t", { 1, 0, 1 }, "#ff00ff");
  test.format<f3d::color_t>("color_t", { 0.1, 0.2, 0.3 }, "0.1,0.2,0.3");
  test.format<f3d::color_t>("color_t", { 0, 0, 0 }, "#000000");
  test.format<f3d::color_t>("color_t", { 1, 0, 1 }, "#ff00ff");
  test.format<f3d::color_t>("color_t",
    { static_cast<double>(171. / 255.), static_cast<double>(205. / 255.),
      static_cast<double>(239. / 255.) },
    "#abcdef");

  test.parse<f3d::direction_t>("direction_t", "+X", { 1, 0, 0 });
  test.parse<f3d::direction_t>("direction_t", "-Y", { 0, -1, 0 });
  test.parse<f3d::direction_t>("direction_t", "+Z", { 0, 0, 1 });
  test.parse<f3d::direction_t>("direction_t", "+xZ", { 1, 0, 1 });
  test.parse<f3d::direction_t>("direction_t", "-XZ", { -1, 0, -1 });
  test.parse<f3d::direction_t>("direction_t", "-X+Z", { -1, 0, 1 });
  test.parse<f3d::direction_t>("direction_t", "-Xy+Z", { -1, -1, +1 });
  test.parse<f3d::direction_t>("direction_t", "-x+Y-Z", { -1, +1, -1 });
  test.parse_expect<f3d::direction_t, parsing_exception>("invalid direction_t", "-K");
  test.parse_expect<f3d::direction_t, parsing_exception>("invalid direction_t", "-y+zx");
  test.parse_expect<f3d::direction_t, parsing_exception>("invalid direction_t", "-x++yz");
  test.parse<f3d::direction_t>("direction_t", "0.1,0.2,0.3", { 0.1, 0.2, 0.3 });
  test.parse<f3d::direction_t>("direction_t", "  0.1,  0.2 , 0.3 ", { 0.1, 0.2, 0.3 });
  test.parse_expect<f3d::color_t, parsing_exception>(
    "incorrect size direction_t", "0.1,0.2,0.3,0.4");
  test.format<f3d::direction_t>("direction_t", { 0.1, 0.2, 0.3 }, "0.1,0.2,0.3");
  test.format<f3d::direction_t>("direction_t", { 0, 0, 0 }, "0,0,0");
  test.format<f3d::direction_t>("direction_t", { +1, 0, 0 }, "+X");
  test.format<f3d::direction_t>("direction_t", { 0, +2, 0 }, "+Y");
  test.format<f3d::direction_t>("direction_t", { 0, 0, +3 }, "+Z");
  test.format<f3d::direction_t>("direction_t", { -1, 0, 0 }, "-X");
  test.format<f3d::direction_t>("direction_t", { 0, -2, 0 }, "-Y");
  test.format<f3d::direction_t>("direction_t", { 0, 0, -3 }, "-Z");
  test.format<f3d::direction_t>("direction_t", { +1, 0, +1 }, "+XZ");
  test.format<f3d::direction_t>("direction_t", { 0, +2, -2 }, "+Y-Z");
  test.format<f3d::direction_t>("direction_t", { -0.1, -0.1, +0.1 }, "-XY+Z");
  test.format<f3d::direction_t>("direction_t", { +0.1, -0.1, +0.2 }, "0.1,-0.1,0.2");

  test.parse<f3d::colormap_t>("colormap_t", "0,0,0,0,1,1,1,1", { 0, 0, 0, 0, 1, 1, 1, 1 });
  test.parse<f3d::colormap_t>(
    "colormap_t", "  -0.0,   0,   0   , 0.0,1,1.0  ,1,  1.0 ", { 0, 0, 0, 0, 1, 1, 1, 1 });
  test.parse<f3d::colormap_t>("colormap_t", "0,#ff00ff,.2,#abcdef,0.8,#0000ff",
    { 0, 1, 0, 1, 0.2, static_cast<double>(171. / 255.), static_cast<double>(205. / 255.),
      static_cast<double>(239. / 255.), 0.8, 0, 0, 1 });
  test.parse_expect<f3d::colormap_t, parsing_exception>(
    "invalid parsing colormap_t", "zero,0,0,0,one,1,1,1");
  test.parse_expect<f3d::colormap_t, parsing_exception>(
    "invalid parsing colormap_t val < [0,1] range", "-1,0,0,0,1,1,1,1");
  test.parse_expect<f3d::colormap_t, parsing_exception>(
    "invalid parsing colormap_t [0,1] < val range", "0,0,0,0,2,1,1,1");
  test.parse_expect<f3d::colormap_t, parsing_exception>(
    "incorrect number of tokens color", "0,0,0,0,1,1,1");
  test.parse_expect<f3d::colormap_t, parsing_exception>(
    "incorrect number of tokens value", "0,0,0,0,1");
  test.parse_expect<f3d::colormap_t, parsing_exception>("invalid color", "0,0,0,0,1,1,1,invalid");

  test.format<f3d::colormap_t>(
    "colormap_t", { 0, 0, 0, 0, 0.5, 0, 1, 1, 0.9, 1, 1, 0 }, "0,#000000,0.5,#00ffff,0.9,#ffff00");
  test.format<f3d::colormap_t>(
    "colormap_t", { 0, 0.1, 0.2, 0.3, 1, 0.3, 0.2, 0.1 }, "0,0.1,0.2,0.3,1,0.3,0.2,0.1");

  test.parse<f3d::transform2d_t>(
    "transform2d_t", "1,0,0,0,-1,0,0,0,1", { 1, 0, 0, 0, -1, 0, 0, 0, 1 });
  test.parse<f3d::transform2d_t>(
    "transform2d_t", "0,0,0,0,0,0,0,0,0", { 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  test.parse<f3d::transform2d_t>(
    "transform2d_t", "0.5,0,0,0,0.5,0,0,0,0.5", { 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5 });
  test.parse<f3d::transform2d_t>("transform2d_t", "scale:0.1", { 0.1, 0, 0, 0, 0.1, 0, 0, 0, 1 });
  test.parse<f3d::transform2d_t>(
    "transform2d_t", "scale:0.1,0.2", { 0.1, 0, 0, 0, 0.2, 0, 0, 0, 1 });
  test.parse<f3d::transform2d_t>(
    "transform2d_t", "translation:0.51,2.1", { 1, 0, 0.51, 0, 1, 2.1, 0, 0, 1 });
  test.parse<f3d::transform2d_t>("transform2d_t", "angle:90.0",
    { std::cos(TestSDKHelpers::Degrees2Radians(90.0)),
      -std::sin(TestSDKHelpers::Degrees2Radians(90.0)), 0,
      std::sin(TestSDKHelpers::Degrees2Radians(90.0)),
      std::cos(TestSDKHelpers::Degrees2Radians(90.0)), 0, 0, 0, 1 });
  test.parse<f3d::transform2d_t>("transform2d_t", "scale:0.1;translation:0.51,2.1;angle:60.0",
    { 0.1 * std::cos(TestSDKHelpers::Degrees2Radians(60.0)),
      0.1 * -std::sin(TestSDKHelpers::Degrees2Radians(60.0)), 0.51,
      0.1 * std::sin(TestSDKHelpers::Degrees2Radians(60.0)),
      0.1 * std::cos(TestSDKHelpers::Degrees2Radians(60.0)), 2.1, 0, 0, 1 });
  test.parse_expect<f3d::transform2d_t, parsing_exception>("vector too small", "1");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "vector too large", "1,2,3,4,5,6,7,8,9,0");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "text in vector notation", "1,2,three,4,5,6,7,8,9");
  test.parse_expect<f3d::transform2d_t, parsing_exception>("invalid argument", "rotation:45.0");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "invalid argument after scale", "scale:2;rotation:45.0");
  test.parse_expect<f3d::transform2d_t, parsing_exception>("no value provided for scale", "scale:");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "no value provided for scale", "scale:,angle:0.5");
  test.parse_expect<f3d::transform2d_t, parsing_exception>("too many scale values", "scale:1,2,3");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "no value provided for translation", "translation:");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "one value provided for translation", "translation:0.5");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "too many values provided for translation", "translation:1,2,3");
  test.parse_expect<f3d::transform2d_t, parsing_exception>("no value provided for angle", "angle:");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "too many angle values", "angle:30.0,45.0,60.0");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "multiple scale transforms", "scale:1,2;scale:3,4");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "multiple translation transforms", "translation:1,2;translation:3,4");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "multiple angle transforms", "angle:30.0;angle:60.0");
  test.parse_expect<f3d::transform2d_t, parsing_exception>(
    "comma-separated options", "scale:0.1,translation:0.51,2.1,angle:60.0");
  test.format<f3d::transform2d_t>(
    "transform2d_t", { 1, 0, 0, 0, -1, 0, 0, 0, 1 }, "1,0,0,0,-1,0,0,0,1");
  test.format<f3d::transform2d_t>(
    "transform2d_t", { 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5 }, "0.5,0,0,0,0.5,0,0,0,0.5");

  test.parse<std::vector<std::string>>(
    "std::vector<std::string>", "foo,bar,baz", { "foo", "bar", "baz" });
  test.parse<std::vector<std::string>>(
    "std::vector<std::string>", "  foo, bar ,   baz ", { "foo", "bar", "baz" });
  test.format<std::vector<std::string>>(
    "std::vector<std::string>", { "foo", "bar", "baz" }, "foo,bar,baz");

  return test.result();
}
