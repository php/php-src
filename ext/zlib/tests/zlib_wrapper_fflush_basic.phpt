--TEST--
Test function fflush() on a zlib stream wrapper
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$filename = "zlib_wrapper_fflush_basic.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written.";
$length = 10;
var_dump(fflush($h));
gzwrite( $h, $str);
gzwrite( $h, $str);
var_dump(fflush($h));
gzclose($h);

$h = gzopen($filename, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($filename);
?>
--EXPECT--
bool(true)
bool(true)
Here is the string to be written.Here is the string to be written.
