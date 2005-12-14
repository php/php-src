--TEST--
imagecreatefromwbmp
--SKIPIF--
<?php
        if (!function_exists('imagecreatefromwbmp')) die("skip gd extension not available\n");
        if (!GD_BUNDLED) die('skip external GD libraries always fail');
?>
--FILE--
<?php
$file = dirname(__FILE__) . '/src.wbmp';

$im2 = imagecreatefromwbmp($file);
echo 'test create from wbmp: ';
echo imagecolorat($im2, 3,3) == 0x0 ? 'ok' : 'failed';
echo "\n";
?>
--EXPECT--
test create from wbmp: ok
