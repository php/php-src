--TEST--
Create an animated GIF with the imagegifanim*() functions
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
?>
--FILE--
<?php
$im = imagecreate(100, 50);
$out = fopen('php://memory', 'w+b');
imagecolorallocate($im, 0, 0, 0);
imagegifanimbegin($im, $out);
imagedestroy($im);

for ($i = -30; $i < 120; $i += 10) {
    $im = imagecreate(100, 50);
    imagecolorallocate($im, 0, 0, 0);
    $color = imagecolorallocate($im, 255, 255, 0);
    $angle = (abs($i/10) % 2) * 45;
    imagefilledarc($im, $i+15,25, 30,30, $angle,360-$angle, $color, IMG_ARC_PIE);
    imagegifanimadd($im, $out, true, 0, 0, 30);
    imagedestroy($im);
}

imagegifanimend($out);

//rewind($out);
//file_put_contents(__FILE__ . '.gif', stream_get_contents($out));

rewind($out);
echo md5(stream_get_contents($out));
fclose($out);
?>
--EXPECT--
acffc98703c521fbe1983efb8a66768e
