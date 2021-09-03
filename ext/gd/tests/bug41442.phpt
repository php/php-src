--TEST--
Bug #41442 (imagegd2() under output control)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagegd2")) {
        die("skip GD2 support unavailable");
    }
?>
--FILE--
<?php

$str = file_get_contents(__DIR__.'/src.gd2');
$res = imagecreatefromstring($str);

/* string */
ob_start();
imagegd2($res);
$str2 = ob_get_clean();
var_dump(imagecreatefromstring($str2));

/* file */
$file = __DIR__."/bug41442.gd2";
imagegd2($res, $file);
$str2 = file_get_contents($file);
var_dump(imagecreatefromstring($str2));

@unlink($file);

echo "Done\n";
?>
--EXPECT--
object(GdImage)#2 (0) {
}
object(GdImage)#2 (0) {
}
Done
