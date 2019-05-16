--TEST--
Font charmap order is deterministic based on order in the font, use the selected encoding
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imagettftext')) die('skip imagettftext() not available');
  if(gd_info()['JIS-mapped Japanese Font Support']) die('skip JIS-mapped Japanese Font Support not supported');
?>
--FILE--
<?php
// this is an Apache Licensed font, see separate LICENSE file
$font = __DIR__.'/Rochester-Regular.otf';

// thank you Helgi
$sample_string = "\xC3\x9E\xC3\xB6";

$im = imagecreatetruecolor(
  100,
  80
);

$white = imagecolorallocate($im, 255, 255, 255);
$black = imagecolorallocate($im, 0, 0, 0);

imagefilledrectangle(
  $im,
  0,
  0,
  100,
  80,
  $white
);

imagettftext(
  $im,
  45,
  0,
  15,
  60,
  $black,
  $font,
  $sample_string
);

$w = imagesx($im);
$h = imagesy($im);
$black_pixels = 0;

for ($y = 0; $y < $h; $y++) {
    for ($x = 0; $x < $w; $x++) {
        $rgb = imagecolorat($im, $x, $y);
        if ($rgb === 0) {
          ++$black_pixels;
        }
    }
}

if ($black_pixels >= 10) {
  printf("SUCCESS %d black pixels\n", $black_pixels);
} else {
  printf("FAIL %d black pixels\n", $black_pixels);
}
imagedestroy($im);
?>
--EXPECTF--
SUCCESS %d black pixels
