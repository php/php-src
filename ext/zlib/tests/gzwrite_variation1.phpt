--TEST--
Test function gzwrite() by calling it when file is opened for reading
--EXTENSIONS--
zlib
--FILE--
<?php

$filename = __DIR__."/004.txt.gz";
$h = gzopen($filename, 'r');
$str = "Here is the string to be written. ";
$length = 10;
var_dump(gzwrite( $h, $str ) );
var_dump(gzread($h, 10));
var_dump(gzwrite( $h, $str, $length ) );
gzclose($h);

?>
--EXPECT--
int(0)
string(10) "When you'r"
int(0)
