--TEST--
imagecreatefromgd2
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }
        if (!function_exists('imagecreatefromgd2')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$file = __DIR__ . '/src.gd2';

$im2 = imagecreatefromgd2($file);
echo 'test create from gd2: ';
echo imagecolorat($im2, 4,4) == 0xffffff ? 'ok' : 'failed';
echo "\n";

$im3 = imagecreatefromgd2part($file, 4,4, 2,2);
echo 'test create from gd2 part: ';
echo imagecolorat($im2, 4,4) == 0xffffff ? 'ok' : 'failed';
echo "\n";
?>
--EXPECT--
test create from gd2: ok
test create from gd2 part: ok
