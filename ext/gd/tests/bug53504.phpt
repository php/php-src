--TEST--
Bug #53504 imagettfbbox/imageftbbox gives incorrect values for bounding box
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
	if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');

	include dirname(__FILE__) . '/func.inc';
	if(version_compare(get_freetype_version(), '2.4.10') == -1) die('skip for freetype < 2.4.10');
?>
--FILE--
<?php
$cwd = dirname(__FILE__);
$font = "$cwd/Tuffy.ttf";

$g = imagecreate(800, 800);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$red = imagecolorallocate($g, 255, 0, 0);
$blue = imagecolorallocate($g, 0, 0, 255);

$tests = [
    // Kerning examples (unfortunately not available in "Tuffy" test font):
    ['fontSize' => 50, 'angle' => 0, 'x' => 20, 'y' => 70, 'text' => 'AV Teg'],
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 350, 'text' => 'AV Teg'],
    ['fontSize' => 50, 'angle' => 40, 'x' => 130, 'y' => 280, 'text' => 'AV Teg'],

    // Shift-Test:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 110, 'text' => 'H-Shift'],

    // Small/single chars:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 220, 'text' => '-'],
    ['fontSize' => 100, 'angle' => 0, 'x' => 430, 'y' => 220, 'text' => ','],
    ['fontSize' => 100, 'angle' => 0, 'x' => 510, 'y' => 220, 'text' => '.'],
    ['fontSize' => 100, 'angle' => 0, 'x' => 590, 'y' => 220, 'text' => '|'],
    ['fontSize' => 100, 'angle' => 0, 'x' => 670, 'y' => 220, 'text' => 'g'],

    // Multi-Line + rotation:
    ['fontSize' => 30, 'angle' => 0, 'x' => 20, 'y' => 400, 'text' => "Multi\nLine\nTest"],
    ['fontSize' => 30, 'angle' => 40, 'x' => 150, 'y' => 420, 'text' => "Multi\nLine\nTest"],
    ['fontSize' => 30, 'angle' => 90, 'x' => 250, 'y' => 340, 'text' => "Multi\nLine\nTest"],

    // Some edge case glyphs:
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 750, 'text' => "iiiiiiiiiiii"],
    ['fontSize' => 50, 'angle' => 90, 'x' => 150, 'y' => 750, 'text' => "~~~~~~~"],
    ['fontSize' => 50, 'angle' => 50, 'x' => 210, 'y' => 750, 'text' => "iiiiiiiiiiii"],
    ['fontSize' => 50, 'angle' => 50, 'x' => 300, 'y' => 750, 'text' => "~~~~~~~"],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 650, 'text' => "iiiiiiiiiiii"],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 750, 'text' => "~~~~~~~"],

    // "Big" test:
    ['fontSize' => 200, 'angle' => 0, 'x' => 400, 'y' => 500, 'text' => "Big"],
];

foreach ($tests as $test) {
    $bbox = imageftbbox($test['fontSize'], $test['angle'], $font, $test['text']);
    vprintf("(%d, %d), (%d, %d), (%d, %d), (%d, %d)\n", $bbox);

    $bboxDrawn = imagefttext($g, $test['fontSize'], $test['angle'],
        $test['x'], $test['y'], $black, $font, $test['text']);

    // check if both bboxes match when adding x/y offset:
    for ($i = 0; $i < count($bbox); $i += 2) {
        if ($bbox[$i] + $test['x'] !== $bboxDrawn[$i]) echo "imageftbbox and imagefttext differ!\n";
        if ($bbox[$i + 1] + $test['y'] !== $bboxDrawn[$i + 1]) echo "imageftbbox and imagefttext differ!\n";
    }

    // draw bounding box:
    imagepolygon($g, $bboxDrawn, 4, $red);

    // draw baseline:
    $width = sqrt(pow($bboxDrawn[2] - $bboxDrawn[0], 2) + pow($bboxDrawn[3] - $bboxDrawn[1], 2));
    imageline($g, $test['x'], $test['y'],
        $test['x'] + $width * cos(deg2rad($test['angle'])),
        $test['y'] - $width * sin(deg2rad($test['angle'])), $blue);
}

imagepng($g, "$cwd/bug53504.png");
?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug53504.png'); ?>
--EXPECTF--
(2, 15), (208, 15), (208, -48), (2, -48)
(15, -1), (15, -208), (-48, -208), (-48, -2)
(11, 11), (169, -122), (129, -171), (-30, -39)
(8, 2), (385, 2), (385, -97), (8, -97)
(7, -37), (51, -37), (51, -46), (7, -46)
(7, 15), (21, 15), (21, -13), (7, -13)
(7, 1), (21, 1), (21, -13), (7, -13)
(8, 0), (17, 0), (17, -95), (8, -95)
(5, 29), (60, 29), (60, -72), (5, -72)
(2, 107), (80, 107), (80, -29), (2, -29)
(70, 81), (131, 31), (43, -74), (-18, -24)
(107, -1), (107, -80), (-29, -80), (-29, -2)
(0, -4), (0, -165), (-47, -165), (-47, -4)
(-19, -2), (-18, -167), (-29, -167), (-29, -2)
(3, -3), (107, -127), (70, -157), (-34, -33)
(-13, -13), (93, -141), (85, -147), (-21, -20)
(4, 0), (165, 0), (165, -47), (4, -47)
(2, -19), (167, -19), (167, -29), (2, -29)
(16, 59), (330, 59), (330, -190), (16, -190)
