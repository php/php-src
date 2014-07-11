--TEST--
Bug #43073 (TrueType bounding box is wrong for angle<>0) freetype >= 2.4.10
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imagettftext')) die('skip imagettftext() not available');

	include dirname(__FILE__) . '/func.inc';
	if(version_compare(get_freetype_version(), '2.4.10') == -1) die('skip for freetype >= 2.4.10');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";
$delta_t = 360.0 / 16; # Make 16 steps around
$g = imagecreate(800, 800);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$x = 100;
$y = 0;
$cos_t = cos(deg2rad($delta_t));
$sin_t = sin(deg2rad($delta_t));
for ($angle = 0.0; $angle < 360.0; $angle += $delta_t) {
  $bbox = imagettftext($g, 24, $angle, 400+$x, 400+$y, $black, $font, 'ABCDEF');
  $s = vsprintf("(%d, %d), (%d, %d), (%d, %d), (%d, %d)\n", $bbox);
  echo $s;
  $temp = $cos_t * $x + $sin_t * $y;
  $y    = $cos_t * $y - $sin_t * $x;
  $x    = $temp;
}
imagepng($g, "$cwd/bug43073.png");
?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug43073.png'); ?>
--EXPECTF--
(500, 400), (610, 400), (610, 376), (500, 376)
(492, 363), (591, 322), (580, 295), (480, 336)
(470, 331), (548, 254), (527, 233), (449, 310)
(439, 309), (483, 202), (461, 193), (416, 299)
(400, 300), (400, 183), (380, 183), (380, 300)
(362, 307), (316, 195), (291, 205), (337, 318)
(330, 329), (246, 244), (224, 265), (308, 350)
(308, 360), (202, 316), (190, 344), (296, 388)
(300, 400), (187, 400), (187, 425), (300, 425)
(306, 437), (195, 483), (206, 510), (318, 464)
(328, 469), (240, 557), (260, 578), (349, 491)
(359, 491), (312, 607), (334, 616), (382, 501)
(400, 500), (400, 618), (419, 618), (419, 500)
(436, 493), (483, 607), (507, 597), (461, 482)
(468, 471), (555, 558), (577, 538), (490, 450)
(490, 440), (600, 485), (611, 457), (502, 412)
