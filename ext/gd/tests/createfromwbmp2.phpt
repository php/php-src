--TEST--
imagecreatefromwbmp with invalid wbmp
--SKIPIF--
<?php
	if (!function_exists('imagecreatefromwbmp')) die("skip gd extension not available\n");
	if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$filename = dirname(__FILE__) . '/_tmp.wbmp';
$fp = fopen($filename,"wb");
if (!$fp) {
	exit("Failed to create <$filename>");
}

//write header
$c = 0;
fputs($fp, chr($c), 1);
fputs($fp, $c, 1);

//write width = 2^32 / 4 + 1
$c = 0x84;
fputs($fp, chr($c), 1);
$c = 0x80;
fputs($fp, chr($c), 1);
fputs($fp, chr($c), 1);
fputs($fp, chr($c), 1);
$c = 0x01;
fputs($fp, chr($c), 1);

/*write height = 4*/
$c = 0x04;
fputs($fp, chr($c), 1);

/*write some data to cause overflow*/
for ($i=0; $i<10000; $i++) {
	fwrite($fp, chr($c), 1);
}

fclose($fp);
$im = imagecreatefromwbmp($filename);
unlink($filename);
?>
--EXPECTF--
Warning: imagecreatefromwbmp(): product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d

Warning: imagecreatefromwbmp(): '%s' is not a valid WBMP file in %s on line %d
