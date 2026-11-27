--TEST--
imagepolygon(), imageopenpolygon(), imagefilledpolygon(): too few points blames the points array
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(20, 20);
foreach (['imagepolygon', 'imageopenpolygon', 'imagefilledpolygon'] as $f) {
    try {
        $f($im, [1, 1, 2, 2], 0xffffff);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        @$f($im, [1, 1, 2, 2], 2, 0xffffff);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
ValueError: imagepolygon(): Argument #2 ($points) must have at least 3 points
ValueError: imagepolygon(): Argument #3 ($num_points_or_color) must be greater than or equal to 3
ValueError: imageopenpolygon(): Argument #2 ($points) must have at least 3 points
ValueError: imageopenpolygon(): Argument #3 ($num_points_or_color) must be greater than or equal to 3
ValueError: imagefilledpolygon(): Argument #2 ($points) must have at least 3 points
ValueError: imagefilledpolygon(): Argument #3 ($num_points_or_color) must be greater than or equal to 3
