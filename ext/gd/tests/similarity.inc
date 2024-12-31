<?php

/**
 * A very simple algorithm for finding the dissimilarity between images,
 * mainly useful for checking lossy compression.
 */

/**
 * Calculates the euclidean distance of two RGB values.
 */
function calc_pixel_distance(int $color1, int $color2): float
{
    return sqrt(
        ((($color1 >> 16) & 0xFF) - (($color2 >> 16) & 0xFF)) ** 2 +
        ((($color1 >>  8) & 0xFF) - (($color2 >>  8) & 0xFF)) ** 2 +
        ((($color1 >>  0) & 0xFF) - (($color2 >>  0) & 0xFF)) ** 2
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
    for ($j = 0, $m = imagesy($image1); $j < $m; $j++) {
        for ($i = 0, $n = imagesx($image1); $i < $n; $i++) {
            $color1 = imagecolorat($image1, $i, $j);
            $color2 = imagecolorat($image2, $i, $j);
            $dissimilarity += calc_pixel_distance($color1, $color2);
        }
    }
    return $dissimilarity;
}
