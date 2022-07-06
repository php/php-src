--TEST--
Apply imagegammacorrect() to a step wedge
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

test_gamma_both(1, 2);
test_gamma_both(1, 1);
test_gamma_both(2, 1);

function test_gamma_both($in, $out)
{
    test_gamma($in, $out, 'imagecreate');
    test_gamma($in, $out, 'imagecreatetruecolor');
}

function test_gamma($in, $out, $constructor)
{
    $im = $constructor(640, 480);
    for ($j = 0; $j < 4; $j++) {
        for ($i = 0; $i < 32; $i++) {
            draw_cell($im, $i, $j);
        }
    }

    imagegammacorrect($im, $in, $out);

    $filename = __DIR__ . DIRECTORY_SEPARATOR
        . "imagegammacorrect_variation2_{$in}_{$out}.png";
    $kind = $constructor === 'imagecreate' ? 'palette' : 'truecolor';
    echo "$kind gamma ($in, $out): ";
    test_image_equals_file($filename, $im);
}

function draw_cell($im, $x, $y)
{
    $x1 = 20 * $x;
    $y1 = 120 * $y;
    $x2 = $x1 + 19;
    $y2 = $y1 + 119;
    $color = cell_color($im, $x, $y);
    imagefilledrectangle($im, $x1,$y1, $x2,$y2, $color);
}

function cell_color($im, $x, $y)
{
    $channel = 8 * $x + 4;
    switch ($y) {
        case 0:
            return imagecolorallocate($im, $channel, $channel, $channel);
        case 1:
            return imagecolorallocate($im, $channel, 0, 0);
        case 2:
            return imagecolorallocate($im, 0, $channel, 0);
        case 3:
            return imagecolorallocate($im, 0, 0, $channel);
    }
}
?>
--EXPECT--
palette gamma (1, 2): The images are equal.
truecolor gamma (1, 2): The images are equal.
palette gamma (1, 1): The images are equal.
truecolor gamma (1, 1): The images are equal.
palette gamma (2, 1): The images are equal.
truecolor gamma (2, 1): The images are equal.
