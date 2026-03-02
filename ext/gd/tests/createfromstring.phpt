--TEST--
imagecreatefromstring
--EXTENSIONS--
gd
--SKIPIF--
<?php
        if (!function_exists('imagepng')) die("skip no imagpng()\n");
?>
--FILE--
<?php
$dir = __DIR__;

$im = imagecreatetruecolor(5,5);
imagefill($im, 0,0, 0xffffff);
imagesetpixel($im, 3,3, 0x0);
imagepng($im, $dir . '/tc.png');

$im_string = file_get_contents(__DIR__ . '/tc.png');
$im = imagecreatefromstring($im_string);
echo 'createfromstring truecolor png: ';
if (imagecolorat($im, 3,3) != 0x0) {
    echo 'failed';
} else {
    echo 'ok';
}
echo "\n";
unlink($dir . '/tc.png');



$im = imagecreate(5,5);
$c1 = imagecolorallocate($im, 255,255,255);
$c2 = imagecolorallocate($im, 255,0,0);
imagefill($im, 0,0, $c1);
imagesetpixel($im, 3,3, $c2);
imagepng($im, $dir . '/p.png');

$im_string = file_get_contents(__DIR__ . '/p.png');
$im = imagecreatefromstring($im_string);

echo'createfromstring palette png: ';

$c = imagecolorsforindex($im, imagecolorat($im, 3,3));
$failed = false;
if ($c['red'] != 255 || $c['green'] != 0 || $c['blue'] != 0) {
    echo 'failed';
} else {
    echo 'ok';
}
echo "\n";
unlink($dir . '/p.png');


//empty string
try {
    imagecreatefromstring('');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
//random string > 12
$im = imagecreatefromstring(' asdf jklp foo');
?>
--EXPECTF--
createfromstring truecolor png: ok
createfromstring palette png: ok

Warning: imagecreatefromstring(): Data is not in a recognized format in %s on line %d

Warning: imagecreatefromstring(): Data is not in a recognized format in %s on line %d
