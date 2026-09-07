--TEST--
imagepng() supports all PNG filter constants with bundled GD
--EXTENSIONS--
gd
--FILE--
<?php
$filters = [
    PNG_FILTER_NONE,
    PNG_FILTER_SUB,
    PNG_FILTER_UP,
    PNG_FILTER_AVG,
    PNG_FILTER_PAETH,
    PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_UP | PNG_FILTER_AVG | PNG_FILTER_PAETH,
];

$image = imagecreatetruecolor(16, 16);
imagealphablending($image, false);
imagesavealpha($image, true);
for ($y = 0; $y < 16; $y++) {
    for ($x = 0; $x < 16; $x++) {
        imagesetpixel($image, $x, $y, imagecolorallocatealpha(
            $image,
            $x * 16,
            $y * 16,
            ($x + $y) * 8,
            ($x + $y) % 128,
        ));
    }
}

foreach ($filters as $filter) {
    ob_start();
    $result = imagepng($image, null, 6, $filter);
    $png = ob_get_clean();
    var_dump($result);
    var_dump(str_starts_with($png, "\x89PNG\r\n\x1a\n"));
    $decoded = imagecreatefromstring($png);
    var_dump($decoded instanceof GdImage);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
