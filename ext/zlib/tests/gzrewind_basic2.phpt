--TEST--
Test function gzrewind() by calling it with its expected arguments when reading
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');

// read to the end of the file
echo "read to the end of the file, then rewind\n";
gzread($h, 10000);
var_dump(gzeof($h));
var_dump(gztell($h));
gzrewind($h);
var_dump(gzeof($h));
var_dump(gztell($h));
echo "first 20 characters=".gzread($h,20)."\n";

gzclose($h);
?>
--EXPECT--
read to the end of the file, then rewind
bool(true)
int(176)
bool(false)
int(0)
first 20 characters=When you're taught t
