--TEST--
Test function gzeof while writing.
--EXTENSIONS--
zlib
--FILE--
<?php

$filename = __DIR__."/gzeof_variation1.txt.gz";
$h = gzopen($filename, 'w');
$str = "Here is the string to be written. ";
$length = 10;
gzwrite( $h, $str );
var_dump(gzeof($h));
gzwrite( $h, $str, $length);
var_dump(gzeof($h));
gzclose($h);
try {
    var_dump(gzeof($h));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
unlink($filename);
?>
--EXPECT--
bool(false)
bool(false)
gzeof(): supplied resource is not a valid stream resource
