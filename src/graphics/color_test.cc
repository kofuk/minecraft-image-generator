// SPDX-License-Identifier: MIT

#include <boost/test/tools/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "graphics/color.hh"

using namespace pixel_terrain;

BOOST_AUTO_TEST_CASE(blend_color) {
    BOOST_TEST((graphics::blend_color(0xffffffaa, 0x00000000) & 0xffffffff) ==
               0xffffffaa);
    BOOST_TEST((graphics::blend_color(0x00000000, 0xffffffff) & 0xffffffff) ==
               0xffffffff);
    BOOST_TEST((graphics::blend_color(0x12345688, 0x65432155) & 0xffffffff) ==
               0x243749B0);
}

BOOST_AUTO_TEST_CASE(blend_ratio) {
    BOOST_TEST((graphics::blend_color(0x6789abcd, 0x12345678, 0.0) &
                0xffffffff) == 0x6789abcd);
    BOOST_TEST((graphics::blend_color(0x6789abcd, 0x12345678, 1.0) &
                0xffffffff) == 0x123456cd);
}

BOOST_AUTO_TEST_CASE(increase_brightness) {
    BOOST_TEST((graphics::increase_brightness(0x505050ff, 5) & 0xffffffff) ==
               0x555555ff);
    BOOST_TEST((graphics::increase_brightness(0xfcfcfcff, 5) & 0xffffffff) ==
               0xffffffff);
    BOOST_TEST((graphics::increase_brightness(0x50fcfcff, 5) & 0xffffffff) ==
               0x55ffffff);
    BOOST_TEST((graphics::increase_brightness(0xfc50fcff, 5) & 0xffffffff) ==
               0xff55ffff);
    BOOST_TEST((graphics::increase_brightness(0xfcfc50ff, 5) & 0xffffffff) ==
               0xffff55ff);

    BOOST_TEST((graphics::increase_brightness(0x555555ff, -5) & 0xffffffff) ==
               0x505050ff);
    BOOST_TEST((graphics::increase_brightness(0x030303ff, -5) & 0xffffffff) ==
               0x000000ff);
    BOOST_TEST((graphics::increase_brightness(0x5a0303ff, -5) & 0xffffffff) ==
               0x550000ff);
    BOOST_TEST((graphics::increase_brightness(0x035a03ff, -5) & 0xffffffff) ==
               0x005500ff);
    BOOST_TEST((graphics::increase_brightness(0x03035aff, -5) & 0xffffffff) ==
               0x000055ff);
}
