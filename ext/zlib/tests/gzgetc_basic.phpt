--TEST--
Test function gzgetc() by calling it with its expected arguments zlib 1.2.5
--EXTENSIONS--
zlib
--SKIPIF--
<?php

include 'func.inc';
if (version_compare(get_zlib_version(), '1.2.5') > 0) {
    die('skip - only for zlib <= 1.2.5');
}
?>
--FILE--
<?php

// note that gzgets is an alias to fgets. parameter checking tests will be
// the same as gzgets

$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');

$count = 0;
while (gzeof($h) === false) {
   $count++;
   echo fgetc( $h );
}

echo "\ncharacters counted=$count\n";
gzclose($h);

?>
--EXPECT--
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me

characters counted=176
