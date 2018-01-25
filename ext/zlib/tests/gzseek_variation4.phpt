--TEST--
Test function gzseek() by calling it with SEEK_SET when writing
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$f = "gzseek_variation5.gz";
$h = gzopen($f, 'w');
$str1 = "This is the first line.";
$str2 = "This is the second line.";
gzwrite($h, $str1);
echo "tell=".gztell($h)."\n";

//seek forwards 20 bytes.
gzseek($h, strlen($str1) + 20, SEEK_SET);
echo "tell=".gztell($h)."\n";
gzwrite($h, $str2);
echo "tell=".gztell($h)."\n";
gzclose($h);
echo "\nreading the output file\n";
$h = gzopen($f, 'r');
echo gzread($h, strlen($str1))."\n";
var_dump(bin2hex(gzread($h, 20)));
echo gzread($h, strlen($str2))."\n";
gzclose($h);
unlink($f);
?>
===DONE===
--EXPECT--
tell=23
tell=43
tell=67

reading the output file
This is the first line.
string(40) "0000000000000000000000000000000000000000"
This is the second line.
===DONE===
