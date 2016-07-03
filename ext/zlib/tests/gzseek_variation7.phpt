--TEST--
Test function gzseek() by calling it with SEEK_END when writing
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php
$f = "gzseek_variation7.gz";
$h = gzopen($f, 'w'); 
$str1 = "This is the first line.";
$str2 = "This is the second line.";
gzwrite($h, $str1);
echo "tell=";
var_dump(gztell($h));

//seek to the end which is not sensible of course.
echo "move to the end of the file\n";
var_dump(gzseek($h, 0, SEEK_END));
echo "tell=";
var_dump(gztell($h));
gzwrite($h, $str2);
echo "tell=";
var_dump(gztell($h));
gzclose($h);
echo "\nreading the output file\n";
$h = gzopen($f, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($f);
?>
===DONE===
--EXPECTF--
tell=int(23)
move to the end of the file

Warning: gzseek(): SEEK_END is not supported in %s on line %d
int(-1)
tell=int(23)
tell=int(47)

reading the output file
This is the first line.This is the second line.
===DONE===
