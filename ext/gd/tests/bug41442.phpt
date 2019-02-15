--TEST--
Bug #41442 (imagegd2() under output control)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}
	if (!function_exists("imagegd2")) {
		die("skip GD2 support unavailable");
	}
?>
--FILE--
<?php

$str = file_get_contents(dirname(__FILE__).'/src.gd2');
$res = imagecreatefromstring($str);

/* string */
ob_start();
imagegd2($res);
$str2 = ob_get_clean();
var_dump(imagecreatefromstring($str2));

/* file */
$file = dirname(__FILE__)."/bug41442.gd2";
imagegd2($res, $file);
$str2 = file_get_contents($file);
var_dump(imagecreatefromstring($str2));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (gd)
resource(%d) of type (gd)
Done
