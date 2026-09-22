--TEST--
Bug #43073 (TrueType bounding box is wrong for angle<>0)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if(!function_exists('imagettftext')) die('skip imagettftext() not available');
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
$exp = [
    [501,400, 611,400, 611,376, 501,376],
    [492,361, 597,319, 588,296, 483,338],
    [470,329, 549,249, 533,233, 453,312],
    [439,307, 481,202, 460,193, 416,297],
    [400,299, 400,187, 376,187, 376,299],
    [361,305, 319,201, 296,210, 338,314],
    [329,327, 249,248, 233,265, 311,344],
    [307,360, 202,316, 193,338, 297,383],
    [299,400, 187,400, 187,424, 299,424],
    [305,438, 201,480, 210,503, 314,461],
    [327,470, 248,548, 265,566, 344,488],
    [360,492, 316,595, 338,604, 383,502],
    [400,501, 400,611, 424,611, 424,501],
    [438,492, 480,597, 503,588, 461,483],
    [470,470, 548,549, 566,532, 488,453],
    [492,439, 595,481, 604,460, 502,416]
];
$cwd = __DIR__;
$font = "$cwd/Tuffy.ttf";
$delta_t = 360.0 / 16; # Make 16 steps around
$g = imagecreate(800, 800);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$red = imagecolorallocate($g, 255, 0, 0);
$x = 100;
$y = 0;
$cos_t = cos(deg2rad($delta_t));
$sin_t = sin(deg2rad($delta_t));
for ($angle = 0.0, $i = 0; $angle < 360.0; $angle += $delta_t, $i++) {
  $bbox = imagettftext($g, 24, (int)$angle, (int)(400+$x), (int)(400+$y), $black, $font, 'ABCDEF');
  imagepolygon($g, $bbox, $red);
  printf("%2d: ", $i);
  for ($j = 0; $j < 8; $j++) {
    if ($bbox[$j] >= $exp[$i][$j] - 1 && $bbox[$j] <= $exp[$i][$j] + 1) {
        echo '.';
    } else {
        echo "(expected {$exp[$i][$j]}, got {$bbox[$j]})";
    }
  }
  echo "\n";
  $temp = $cos_t * $x + $sin_t * $y;
  $y    = $cos_t * $y - $sin_t * $x;
  $x    = $temp;
}
imagepng($g, "$cwd/bug43073.png");
?>
--CLEAN--
<?php @unlink(__DIR__ . '/bug43073.png'); ?>
--EXPECT--
 0: ........
 1: ........
 2: ........
 3: ........
 4: ........
 5: ........
 6: ........
 7: ........
 8: ........
 9: ........
10: ........
11: ........
12: ........
13: ........
14: ........
15: ........
