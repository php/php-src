--TEST--
Test function gzgetc() by calling it with its expected arguments
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php

// note that gzgets is an alias to fgets. parameter checking tests will be
// the same as gzgets

$f = dirname(__FILE__)."/004.txt.gz";
$h = gzopen($f, 'r');

$count = 0;
while (gzeof($h) === false) {
   $count++;
   echo fgetc( $h );
}

echo "\ncharacters counted=$count\n";
gzclose($h);

?>
===DONE===
--EXPECT--
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me

characters counted=176
===DONE===