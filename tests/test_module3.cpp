#include "../src/basic_graphics.h"
#include <iostream>
#include <cassert>

using namespace gwbasic;

void test_pset_and_point() {
    BasicGraphics gfx(320, 200, ScreenMode::MediumRes);
    gfx.cls(0);

    assert(gfx.point(50, 50) == 0);
    gfx.pset(50, 50, 2);
    assert(gfx.point(50, 50) == 2);

    gfx.preset(50, 50); // Reset to background
    assert(gfx.point(50, 50) == 0);

    // Out of bounds safety
    gfx.pset(-5, -5, 1);
    gfx.pset(500, 500, 1);
    assert(gfx.point(500, 500) == 0);

    std::cout << "[PASS] test_pset_and_point\n";
}

void test_line_and_box() {
    BasicGraphics gfx(320, 200, ScreenMode::MediumRes);
    gfx.cls(0);

    // Draw horizontal line
    gfx.line(10, 20, 30, 20, 3);
    for (int x = 10; x <= 30; ++x) {
        assert(gfx.point(x, 20) == 3);
    }
    assert(gfx.point(9, 20) == 0);
    assert(gfx.point(31, 20) == 0);

    // Draw solid filled box
    gfx.box(40, 40, 50, 50, 1, true);
    for (int y = 40; y <= 50; ++y) {
        for (int x = 40; x <= 50; ++x) {
            assert(gfx.point(x, y) == 1);
        }
    }
    assert(gfx.point(39, 40) == 0);
    assert(gfx.point(51, 50) == 0);

    std::cout << "[PASS] test_line_and_box\n";
}

void test_paint_flood_fill() {
    BasicGraphics gfx(320, 200, ScreenMode::MediumRes);
    gfx.cls(0);

    // Draw a closed box boundary with color 1 (Border)
    gfx.box(10, 10, 30, 30, 1, false);

    // Ensure the center is empty (color 0)
    assert(gfx.point(20, 20) == 0);
    // Ensure outside is empty
    assert(gfx.point(5, 5) == 0);

    // PAINT inside with fillColor = 2, borderColor = 1
    gfx.paint(20, 20, 2, 1);

    // Inside must be filled with color 2
    for (int y = 11; y < 30; ++y) {
        for (int x = 11; x < 30; ++x) {
            assert(gfx.point(x, y) == 2);
        }
    }

    // Border must remain color 1
    assert(gfx.point(10, 10) == 1);
    assert(gfx.point(30, 30) == 1);

    // Outside must remain color 0
    assert(gfx.point(5, 5) == 0);
    assert(gfx.point(35, 35) == 0);

    std::cout << "[PASS] test_paint_flood_fill (Scanline algorithm from ADVGRP.ASM)\n";
}

void test_circle() {
    BasicGraphics gfx(320, 200, ScreenMode::MediumRes);
    gfx.cls(0);

    gfx.circle(100, 100, 20, 2);
    // Right point (120, 100) and left point (80, 100) should be colored
    assert(gfx.point(120, 100) == 2);
    assert(gfx.point(80, 100) == 2);
    // Center (100, 100) should be empty
    assert(gfx.point(100, 100) == 0);

    std::cout << "[PASS] test_circle\n";
}

int main() {
    std::cout << "=== Running GW-BASIC Module 3 Tests (GENGRP & ADVGRP) ===\n";
    test_pset_and_point();
    test_line_and_box();
    test_paint_flood_fill();
    test_circle();
    std::cout << "=== ALL MODULE 3 TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}
