--TEST--
Bug #75124 (gdImageGrayScale() may produce colors)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.5', '<')) {
    die('skip only for bundled libgd or external libgd >= 2.2.5');
}
?>
--FILE--
<?php
$im = imagecreatefrompng(__DIR__ . '/bug75124.png');
var_dump(imageistruecolor($im));
imagefilter($im, IMG_FILTER_GRAYSCALE);
for ($i = 0, $width = imagesx($im); $i < $width; $i ++) {
    for ($j = 0, $height = imagesy($im); $j < $height; $j++) {
        $color = imagecolorat($im, $i, $j);
        $red = ($color >> 16) & 0xff;
        $green = ($color >> 8) & 0xff;
        $blue = $color & 0xff;
        if ($red != $green || $green != $blue) {
            echo "non grayscale pixel detected\n";
            break 2;
        }
    }
}
?>
--EXPECT--
bool(true)
