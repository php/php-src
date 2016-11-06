--TEST--
Bug #53156 (imagerectangle problem with point ordering)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3', '<')) {
    die("skip test requires GD 2.3 or newer");
}
?>
--FILE--
<?php
function draw_and_check_pixel($x, $y)
{
    global $img, $black, $red;
    
    echo (imagecolorat($img, $x, $y) === $black) ? '+' : '-';
    imagesetpixel($img, $x, $y, $red);
}

function draw_and_check_rectangle($x1, $y1, $x2, $y2)
{
    global $img, $black;
    
    echo 'Rectangle: ';
    imagerectangle($img, $x1, $y1, $x2, $y2, $black);
    $x = ($x1 + $x2) / 2;
    $y = ($y1 + $y2) / 2;
    draw_and_check_pixel($x,  $y1);
    draw_and_check_pixel($x1, $y);
    draw_and_check_pixel($x,  $y2);
    draw_and_check_pixel($x2, $y);
    echo PHP_EOL;
}

$img = imagecreate(110, 210);
$bgnd  = imagecolorallocate($img, 255, 255, 255);
$black = imagecolorallocate($img,   0,   0,   0);
$red   = imagecolorallocate($img, 255,   0,   0);

draw_and_check_rectangle( 10,  10,  50,  50);
draw_and_check_rectangle( 50,  60,  10, 100);
draw_and_check_rectangle( 50, 150,  10, 110);
draw_and_check_rectangle( 10, 200,  50, 160);
imagesetthickness($img, 4);
draw_and_check_rectangle( 60,  10, 100,  50);
draw_and_check_rectangle(100,  60,  60, 100);
draw_and_check_rectangle(100, 150,  60, 110);
draw_and_check_rectangle( 60, 200, 100, 160);

//imagepng($img, __DIR__ . '/bug53156.png'); // debug
?>
--EXPECT--
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
Rectangle: ++++
