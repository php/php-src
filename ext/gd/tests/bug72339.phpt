--TEST--
Bug #72339 Integer Overflow in _gd2GetHeader() resulting in heap overflow
--SKIPIF--
<?php
if (!function_exists("imagecreatefromgd2")) print "skip";
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
	die("skip test requires GD 2.2.2 or higher");
}
?>
--FILE--
<?php
$fname = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug72339.gd";

$fh = fopen($fname, "w");
fwrite($fh, "gd2\x00");
fwrite($fh, pack("n", 2));
fwrite($fh, pack("n", 1));
fwrite($fh, pack("n", 1));
fwrite($fh, pack("n", 0x40));
fwrite($fh, pack("n", 2));
fwrite($fh, pack("n", 0x5AA0)); // Chunks Wide
fwrite($fh, pack("n", 0x5B00)); // Chunks Vertically
fwrite($fh, str_repeat("\x41\x41\x41\x41", 0x1000000)); // overflow data
fclose($fh);

$im = imagecreatefromgd2($fname);

if ($im) {
	imagedestroy($im);
}
unlink($fname);

?>
--EXPECTF--
Warning: imagecreatefromgd2(): product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %sbug72339.php on line %d

Warning: imagecreatefromgd2(): '%sbug72339.gd' is not a valid GD2 file in %sbug72339.php on line %d
