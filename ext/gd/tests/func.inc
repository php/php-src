<?php

function get_gd_version()
{
    return GD_VERSION;
}

function get_php_info()
{
    ob_start();
    phpinfo();
    $info = ob_get_contents();
    ob_end_clean();

    return $info;
}

function get_freetype_version()
{
    $version = 0;

    if (preg_match(',FreeType Version => (\d+\.\d+\.\d+),s', get_php_info(), $match)) {
        $version = $match[1];
    }

    return $version;
}

function get_libjpeg_version()
{
    $version = 0;

    if (preg_match(',libJPEG Version => ([a-z0-9]+),s', get_php_info(), $match)) {
        $version = $match[1];
    }

    return $version;
}

function get_libpng_version()
{
    $version = 0;

    if (preg_match(',libPNG Version => (\d+\.\d+\.\d+),s', get_php_info(), $match)) {
        $version = $match[1];
    }

    return $version;
}

function get_libxpm_version()
{
    $version = 0;

    if (preg_match(',libXpm Version => (\d+),s', get_php_info(), $match)) {
        $version = $match[1];
    }

    return $version;
}

/**
 * Tests that an in-memory image equals a PNG file.
 *
 * It checks for equal image sizes, and whether any pixels are different.
 * The textual result is printed, so the EXPECT section should contain the line
 * "The images are equal."
 *
 * If the PNG file does not exist, or the images are not equal, a diagnostic
 * message is printed, and the actual file is stored right beside the temporary
 * .php test file with the extension .out.png, to be able to manually inspect
 * the result.
 */
function test_image_equals_file(string $filename, GdImage $actual): void
{
    if (!file_exists($filename)) {
        echo "The expected image does not exist.\n";
        save_actual_image($actual);
        return;
    }
    $expected = imagecreatefrompng($filename);
    test_image_equals_image($expected, $actual, true);
}

/**
 * Tests that an in-memory image equals another in-memory image.
 *
 * It checks for equal image sizes, and whether any pixels are different.
 * The textual result is printed, so the EXPECT section should contain the line
 * "The images are equal."
 *
 * If the images are not equal, a diagnostic message is printed.
 */
function test_image_equals_image(GdImage $expected, GdImage $actual, bool $save_actual = false): void
{
    assert(imageistruecolor($expected) && imageistruecolor($actual));
    $exp_x = imagesx($expected);
    $exp_y = imagesy($expected);
    $act_x = imagesx($actual);
    $act_y = imagesy($actual);
    if ($exp_x != $act_x || $exp_y != $act_y) {
        echo "The image size differs: expected {$exp_x}x{$exp_y}, got {$act_x}x{$act_y}.\n";
        if ($save_actual) {
            save_actual_image($actual);
        }
        return;
    }
    $pixels_changed = 0;
    for ($y = 0; $y < $exp_y; $y++) {
        for ($x = 0; $x < $exp_x; $x ++) {
            $exp_c = imagecolorat($expected, $x, $y);
            $act_c = imagecolorat($actual, $x, $y);
            if ($exp_c != $act_c) {
                $pixels_changed++;
            }
        }
    }
    if (!$pixels_changed) {
        echo "The images are equal.\n";
    } else {
        echo "The images differ in {$pixels_changed} pixels.\n";
        if ($save_actual) {
            save_actual_image($actual);
        }
    }
}

/**
 * Saves an actual image to disk.
 *
 * The image is saved right beside the temporary .php test file with the
 * extension .out.png.
 */
function save_actual_image(GdImage $image): void
{
    $pathinfo = pathinfo($_SERVER['SCRIPT_FILENAME']);
    $filename = "{$pathinfo['dirname']}/{$pathinfo['filename']}.out.png";
    imagepng($image, $filename);
}

/**
 * Replicates write errors to the output log, but by catching
 * and formatting exceptions instead so they have a consistent
 * output
 */

function trycatch_dump(...$tests) {
    foreach ($tests as $test) {
        try {
            var_dump($test());
        }
        catch (\Error $e) {
            echo '!! [' . get_class($e) . '] ' . $e->getMessage() . "\n";
        }
    }
}

/**
 * Calculate the mean squared error (in range 0..255**2)
 */
function mse(GdImage $image1, GdImage $image2): float
{
    assert(imageistruecolor($image1) && imageistruecolor($image2));
    assert(imagesx($image1) === imagesx($image2) && imagesy($image1) === imagesy($image2));
    $e = $count = 0;
    for ($j = 0, $m = imagesy($image1); $j < $m; $j++) {
        for ($i = 0, $n = imagesx($image1); $i < $n; $i++) {
            $c1 = imagecolorat($image1, $i, $j);
            $c2 = imagecolorat($image2, $i, $j);
            $e += ((($c1 >> 16) & 255) - (($c2 >> 16) & 255)) ** 2
                + ((($c1 >>  8) & 255) - (($c2 >>  8) & 255)) ** 2
                + ((($c1 >>  0) & 255) - (($c2 >>  0) & 255)) ** 2;
            $count += 3;
        }
    }
    return $e / $count;
}
