--TEST--
Bug #53504 imagettfbbox/imageftbbox gives incorrect values for bounding box
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if(!function_exists('imageftbbox')) die('skip imageftbbox() not available');
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
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
    ['fontSize' => 50, 'angle' => 0, 'x' => 20, 'y' => 70, 'text' => 'AV Teg', 'exp' => [2,14, 209,14, 209,-47, 2,-47]],
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 350, 'text' => 'AV Teg', 'exp' => [14,-2, 14,-209, -47,-209, -47,-2]],
    ['fontSize' => 50, 'angle' => 40, 'x' => 130, 'y' => 280, 'text' => 'AV Teg', 'exp' => [11,9, 169,-123, 129,-170, -28,-37]],

    // Shift-Test:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 110, 'text' => 'H-Shift', 'exp' => [8,1, 393,1, 393,-97, 8,-97]],

    // Small/single chars:
    ['fontSize' => 100, 'angle' => 0, 'x' => 350, 'y' => 220, 'text' => '-', 'exp' => [7,-36, 56,-36, 56,-46, 7,-46]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 430, 'y' => 220, 'text' => ',', 'exp' => [6,14, 25,14, 25,-13, 6,-13]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 510, 'y' => 220, 'text' => '.', 'exp' => [6,0, 26,0, 26,-13, 6,-13]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 590, 'y' => 220, 'text' => '|', 'exp' => [8,0, 23,0, 23,-95, 8,-95]],
    ['fontSize' => 100, 'angle' => 0, 'x' => 670, 'y' => 220, 'text' => 'g', 'exp' => [4,29, 66,29, 66,-71, 4,-71]],

    // Multi-Line + rotation:
    ['fontSize' => 30, 'angle' => 0, 'x' => 20, 'y' => 400, 'text' => "Multi\nLine\nTest", 'exp' => [1,84, 81,84, 81,-28, 1,-28]],
    ['fontSize' => 30, 'angle' => 40, 'x' => 150, 'y' => 420, 'text' => "Multi\nLine\nTest", 'exp' => [55,63, 116,12, 43,-74, -17,-22]],
    ['fontSize' => 30, 'angle' => 90, 'x' => 250, 'y' => 340, 'text' => "Multi\nLine\nTest", 'exp' => [84,-1, 84,-81, -28,-81, -28,-1]],

    // Some edge case glyphs:
    ['fontSize' => 50, 'angle' => 90, 'x' => 70, 'y' => 750, 'text' => "iiiiiiiiiiii", 'exp' => [0,-4, 0,-165, -46,-165, -46,-4]],
    ['fontSize' => 50, 'angle' => 90, 'x' => 150, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [-18,-1, -18,-168, -27,-168, -27,-1]],
    ['fontSize' => 50, 'angle' => 50, 'x' => 210, 'y' => 750, 'text' => "iiiiiiiiiiii", 'exp' => [2,-3, 108,-129, 73,-159, -32,-33]],
    ['fontSize' => 50, 'angle' => 50, 'x' => 300, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [-12,-13, 94,-140, 86,-146, -20,-19]],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 650, 'text' => "iiiiiiiiiiii", 'exp' => [4,0, 165,0, 165,-46, 4,-46]],
    ['fontSize' => 50, 'angle' => 0, 'x' => 430, 'y' => 750, 'text' => "~~~~~~~", 'exp' => [1,-18, 168,-18, 168,-27, 1,-27]],

    // "Big" test:
    ['fontSize' => 200, 'angle' => 0, 'x' => 400, 'y' => 500, 'text' => "Big", 'exp' => [15,58, 342,58, 342,-189, 15,-189]],
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
        if (abs($bbox[$i] + $test['x'] - $bboxDrawn[$i]) > 1) echo "imageftbbox and imagefttext differ!\n";
        if (abs($bbox[$i + 1] + $test['y'] - $bboxDrawn[$i + 1]) > 1) echo "imageftbbox and imagefttext differ!\n";
    }

    // draw bounding box:
    imagepolygon($g, $bboxDrawn, $red);

    // draw baseline:
    $width = sqrt(pow($bboxDrawn[2] - $bboxDrawn[0], 2) + pow($bboxDrawn[3] - $bboxDrawn[1], 2));
    imageline($g, $test['x'], $test['y'],
        $test['x'] + (int)($width * cos(deg2rad($test['angle']))),
        $test['y'] - (int)($width * sin(deg2rad($test['angle']))), $blue);
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
