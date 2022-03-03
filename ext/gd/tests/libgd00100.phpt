--TEST--
libgd #100 (spurious horizontal line drawn by gdImageFilledPolygon)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(256, 256);

$white   = imagecolorallocatealpha($im, 255, 255, 255, 10);
$black   = imagecolorallocatealpha($im,   0,   0,   0, 10);
$red     = imagecolorallocatealpha($im, 255,   0,   0, 10);
$green   = imagecolorallocatealpha($im,   0, 255,   0, 10);
$blue    = imagecolorallocatealpha($im,   0,   0, 255, 10);
$yellow  = imagecolorallocatealpha($im, 255, 255,   0, 10);
$cyan    = imagecolorallocatealpha($im,   0, 255, 255, 10);
$magenta = imagecolorallocatealpha($im, 255,   0, 255, 10);
$purple  = imagecolorallocatealpha($im, 100,   0, 100, 10);

imagefilledrectangle($im, 0, 0, 255, 255, $white);

// M (bridge)
$top = 240;
$bot = 255;
$d = 30;
$x = 100;
$points = array(
  $x,      $top,
  $x+2*$d, $top,
  $x+2*$d, $bot,
  $x+$d,   (int) (($top+$bot)/2),
  $x,      $bot
);
imagefilledpolygon($im, $points, $yellow);

// left-facing M not on baseline
$top = 40;
$bot = 70;
$left = 120;
$right = 180;
$points = array(
  $left,  $top,
  $right, $top,
  $right, $bot,
  $left,  $bot,
  ($left+$right)/2, ($top+$bot)/2
);
imagefilledpolygon($im, $points, $purple);

// left-facing M on baseline
$top = 240;
$bot = 270;
$left = 20;
$right = 80;
$points = array(
  $left,  $top,
  $right, $top,
  $right, $bot,
  $left,  $bot,
  ($left+$right)/2, ($top+$bot)/2
);
imagefilledpolygon($im, $points, $magenta);

// left-facing M on ceiling
$top = -15;
$bot = 15;
$left = 20;
$right = 80;
$points = array(
  $left,  $top,
  $right, $top,
  $right, $bot,
  $left,  $bot,
  ($left+$right)/2, ($top+$bot)/2
);
imagefilledpolygon($im, $points, $blue);

$d = 30;
$x = 150;
$y = 150;
$diamond = array($x-$d, $y, $x, $y+$d, $x+$d, $y, $x, $y-$d);
imagefilledpolygon($im, $diamond, $green);

$x = 180;
$y = 225;
$diamond = array($x-$d, $y, $x, $y+$d, $x+$d, $y, $x, $y-$d);
imagefilledpolygon($im, $diamond, $red);

$x = 225;
$y = 255;
$diamond = array($x-$d, $y, $x, $y+$d, $x+$d, $y, $x, $y-$d);
imagefilledpolygon($im, $diamond, $cyan);

// M (bridge) not touching bottom boundary
$top = 100;
$bot = 150;
$x = 30;
$points = array(
  $x,      $top,
  $x+2*$d, $top,
  $x+2*$d, $bot,
  $x+$d,   ($top+$bot)/2,
  $x,      $bot
);
imagefilledpolygon($im, $points, $black);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/libgd00100.png', $im);

imagedestroy($im);
?>
--EXPECT--
The images are equal.
