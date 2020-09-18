--TEST--
Bug #53504 imagettfbbox/imageftbbox gives incorrect values for bounding box
--SKIPIF--
<?php
    if(!extension_loaded('gd')){ die('skip gd extension not available'); }
    if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');
?>
--FILE--
<?php
$cwd = __DIR__;
$font = "$cwd/Tuffy.ttf";

$g = imagecreate(800, 800);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$red = imagecolorallocate($g, 255, 0, 0);
$blue = imagecolorallocate($g, 0, 0, 255);

$tests = [
    // Kerning examples (unfortunately not available in "Tuffy" test font):
    ['fontSize' => 50, 'angle' => 0, 'x' => 20, 'y' => 70, 'text' => 'AV Teg', 'exp' => [2,15, 208,15, 208,-48, 2,-48]],
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 350, 'text' => 'AV Teg', 'exp' => [15,-1, 15,-208, -48,-208, -48,-2]],
    ['fontSize' => 50, 'angle' => 40, 'x' => 130, 'y' => 280, 'text' => 'AV Teg', 'exp' => [11,11, 169,-122, 129,-171, -30,-39]],

    // Shift-Test:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 110, 'text' => 'H-Shift', 'exp' => [8,2, 386,2, 386,-97, 8,-97]],

    // Small/single chars:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 220, 'text' => '-', 'exp' => [7,-37, 51,-37, 51,-46, 7,-46]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 430, 'y' => 220, 'text' => ',', 'exp' => [7,15, 21,15, 21,-13, 7,-13]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 510, 'y' => 220, 'text' => '.', 'exp' => [7,1, 21,1, 21,-13, 7,-13]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 590, 'y' => 220, 'text' => '|', 'exp' => [8,0, 17,0, 17,-95, 8,-95]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 670, 'y' => 220, 'text' => 'g', 'exp' => [5,29, 60,29, 60,-72, 5,-72]],

    // Multi-Line + rotation:
    ['fontSize' => 30, 'angle' => 0, 'x' => 20, 'y' => 400, 'text' => "Multi\nLine\nTest", 'exp' => [2,107, 80,107, 80,-29, 2,-29]],
    ['fontSize' => 30, 'angle' => 40, 'x' => 150, 'y' => 420, 'text' => "Multi\nLine\nTest", 'exp' => [70,81, 131,31, 43,-74, -18,-24]],
    ['fontSize' => 30, 'angle' => 90, 'x' => 250, 'y' => 340, 'text' => "Multi\nLine\nTest", 'exp' => [107,-1, 107,-80, -29,-80, -29,-2]],

    // Some edge case glyphs:
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 750, 'text' => "iiiiiiiiiiii", 'exp' => [0,-4, 0,-165, -47,-165, -47,-4]],
    ['fontSize' => 50, 'angle' => 90, 'x' => 150, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [-19,-2, -18,-167, -29,-167, -29,-2]],
    ['fontSize' => 50, 'angle' => 50, 'x' => 210, 'y' => 750, 'text' => "iiiiiiiiiiii", 'exp' => [3,-3, 107,-127, 70,-157, -34,-33]],
    ['fontSize' => 50, 'angle' => 50, 'x' => 300, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [-13,-13, 93,-141, 85,-147, -21,-20]],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 650, 'text' => "iiiiiiiiiiii", 'exp' => [4,0, 165,0, 165,-47, 4,-47]],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [2,-19, 167,-19, 167,-29, 2,-29]],

    // "Big" test:
    ['fontSize' => 200, 'angle' => 0, 'x' => 400, 'y' => 500, 'text' => "Big", 'exp' => [16,59, 329,59, 329,-190, 16,-190]],
];

foreach ($tests as $testnum => $test) {
    $bbox = imageftbbox($test['fontSize'], $test['angle'], $font, $test['text']);
    printf('%2d: ', $testnum);
    for ($i = 0; $i < 8; $i++) {
        $exp = $test['exp'][$i];
        if ($bbox[$i] >= $exp - 2 && $bbox[$i] <= $exp + 2) {
            echo '.';
        } else {
            echo "(expected $exp, got $bbox[$i])";
        }
    }
    echo "\n";

    $bboxDrawn = imagefttext($g, $test['fontSize'], $test['angle'],
        $test['x'], $test['y'], $black, $font, $test['text']);

    // check if both bboxes match when adding x/y offset:
    for ($i = 0; $i < count($bbox); $i += 2) {
        if ($bbox[$i] + $test['x'] !== $bboxDrawn[$i]) echo "imageftbbox and imagefttext differ!\n";
        if ($bbox[$i + 1] + $test['y'] !== $bboxDrawn[$i + 1]) echo "imageftbbox and imagefttext differ!\n";
    }

    // draw bounding box:
    imagepolygon($g, $bboxDrawn, $red);

    // draw baseline:
    $width = sqrt(pow($bboxDrawn[2] - $bboxDrawn[0], 2) + pow($bboxDrawn[3] - $bboxDrawn[1], 2));
    imageline($g, $test['x'], $test['y'],
        $test['x'] + $width * cos(deg2rad($test['angle'])),
        $test['y'] - $width * sin(deg2rad($test['angle'])), $blue);
}

imagepng($g, "$cwd/bug53504.png");
?>
--CLEAN--
<?php @unlink(__DIR__ . '/bug53504.png'); ?>
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
16: ........
17: ........
18: ........
