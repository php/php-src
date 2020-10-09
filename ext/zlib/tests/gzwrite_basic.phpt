--TEST--
Test function gzwrite() by calling it with its expected arguments
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$filename = "gzwrite_basic.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
$length = 10;
var_dump(gzwrite( $h, $str ) );
var_dump(gzwrite( $h, $str, $length ) );
gzclose($h);

$h = gzopen($filename, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($filename);
?>
--EXPECT--
int(34)
int(10)
Here is the string to be written. Here is th
