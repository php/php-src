--TEST--
gzopen(), gzread(), gzwrite()
--SKIPIF--
<?php # vim600:syn=php:
if (!extension_loaded("zlib")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$filename = tempnam("/tmp", "phpt");

$fp = gzopen($filename, "wb");
gzwrite($fp, $original);
var_dump(strlen($original));
var_dump(gztell($fp));
fclose($fp);

$fp = gzopen($filename, "rb");
$data = gzread($fp, strlen($original));
if ($data == $original) {
	echo "Strings are equal\n";
} else {
	echo "Strings are not equal\n";
	var_dump($data);
}
gzclose($fp);
unlink($filename);
?>
--EXPECT--
int(36864)
int(36864)
Strings are equal
