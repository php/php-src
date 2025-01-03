--TEST--
Different quantization methods produce similar results
--EXTENSIONS--
gd
--FILE--
<?php
require_once __DIR__ . "/similarity.inc";

$im1 = imagecreatefromjpeg(__DIR__ . "/conv_test.jpg");
imagetruecolortopalettesetmethod($im1, IMG_QUANT_DEFAULT);
imagetruecolortopalette($im1, false, 64);
imagepalettetotruecolor($im1);

$im2 = imagecreatefromjpeg(__DIR__ . "/conv_test.jpg");
imagetruecolortopalettesetmethod($im2, IMG_QUANT_NEUQUANT);
@imagetruecolortopalette($im2, false, 64); // NeuQuant emits diagnostic notices
imagepalettetotruecolor($im2);

var_dump(calc_image_dissimilarity($im1, $im2) < 5e5);
?>
--EXPECT--
bool(true)
