--TEST--
Test function gzread() by calling it while file open for writing
--EXTENSIONS--
zlib
--FILE--
<?php

$filename = "gzread_variation1.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
var_dump(gzread($h, 100));
gzwrite( $h, $str);
var_dump(gzread($h, 100));
gzrewind($h);
var_dump(gzread($h, 100));
gzclose($h);

$h = gzopen($filename, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($filename);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
Here is the string to be written. 
