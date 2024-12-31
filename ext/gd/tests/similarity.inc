<?php

/**
 * A very simple algorithm for finding the dissimilarity between images,
 * mainly useful for checking lossy compression.
 */

/**
 * Gets the individual components of an RGB value.
 */
function get_rgb(int $color, &$red, &$green, &$blue): void
{
    // assumes $color is an RGB value
    $red = ($color >> 16) & 0xFF;
    $green = ($color >> 8) & 0xFF;
    $blue = $color & 0xFF;
}

/**
 * Calculates the euclidean distance of two RGB values.
 */
function calc_pixel_distance(int $color1, int $color2): float
{
    get_rgb($color1, $red1, $green1, $blue1);
    get_rgb($color2, $red2, $green2, $blue2);
    return sqrt(
        pow($red1 - $red2, 2) + pow($green1 - $green2, 2) + pow($blue1 - $blue2, 2)
    );
}

/**
 * Calculates dissimilarity of two images.
 *
 * The dissimilarity. 0 means the images are identical. The higher
 * the value, the more dissimilar are the images.
 */
function calc_image_dissimilarity(GdImage $image1, GdImage $image2): float
{
    // assumes image1 and image2 have same width and height
    $dissimilarity = 0;
    for ($i = 0, $n = imagesx($image1); $i < $n; $i++) {
        for ($j = 0, $m = imagesy($image1); $j < $m; $j++) {
            $color1 = imagecolorat($image1, $i, $j);
            $color2 = imagecolorat($image2, $i, $j);
            $dissimilarity += calc_pixel_distance($color1, $color2);
        }
    }
    return $dissimilarity;
}
