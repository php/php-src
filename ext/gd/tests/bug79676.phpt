--TEST--
Bug #79676 (imagescale adds black border with IMG_BICUBIC)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
function test($image, $desc)
{
    echo "$desc - Test Result: ",
        (imagecolorat($image, imagesx($image) - 1 , imagesy($image) - 1) != 0x000000 ? 'pass' : 'fail'),
        PHP_EOL;
}

$size = 32;
$src = imagecreatetruecolor($size, $size);
imagefilledrectangle($src, 0, 0, $size - 1 , $size - 1, 0xff00ff);

test($src, 'No scaling');
test(imagescale($src, $size * 2, $size * 2), 'Scale 200%, default mode');
test(imagescale($src, $size / 2, $size / 2), 'Scale 50%, default mode');
test(imagescale($src, $size * 2, $size * 2, IMG_BICUBIC), 'Scale 200%, IMG_BICUBIC mode');
test(imagescale($src, $size / 2, $size / 2, IMG_BICUBIC), 'Scale 50%, IMG_BICUBIC mode');
?>
--EXPECT--
No scaling - Test Result: pass
Scale 200%, default mode - Test Result: pass
Scale 50%, default mode - Test Result: pass
Scale 200%, IMG_BICUBIC mode - Test Result: pass
Scale 50%, IMG_BICUBIC mode - Test Result: pass
