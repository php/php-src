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
$red = imagecolorallocate($g, 255, 0, 0);
$x = 100;
$y = 0;
$cos_t = cos(deg2rad($delta_t));
$sin_t = sin(deg2rad($delta_t));
for ($angle = 0.0; $angle < 360.0; $angle += $delta_t) {
  $bbox = imagettftext($g, 24, $angle, 400+$x, 400+$y, $black, $font, 'ABCDEF');
  imagepolygon($g, $bbox, 4, $red);
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
(501, 400), (611, 400), (611, 376), (501, 376)
(492, 361), (595, 319), (586, 296), (483, 338)
(470, 329), (549, 251), (531, 233), (453, 312)
(439, 307), (481, 204), (458, 195), (416, 297)
(400, 299), (400, 189), (376, 189), (376, 299)
(361, 307), (319, 204), (296, 213), (338, 316)
(329, 329), (251, 250), (233, 267), (311, 346)
(307, 360), (204, 318), (195, 341), (297, 383)
(299, 400), (189, 400), (189, 424), (299, 424)
(307, 438), (204, 480), (213, 503), (316, 461)
(329, 470), (250, 548), (267, 566), (346, 488)
(360, 492), (318, 595), (341, 604), (383, 502)
(400, 501), (400, 611), (424, 611), (424, 501)
(438, 492), (480, 595), (503, 586), (461, 483)
(470, 470), (548, 549), (566, 532), (488, 453)
(492, 439), (595, 481), (604, 458), (502, 416)
