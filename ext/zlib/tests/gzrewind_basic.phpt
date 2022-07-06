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
echo "test rewind before doing anything\n";
var_dump(gzrewind($h));
var_dump(gztell($h));
echo "\nfirst 30 characters=".gzread($h, 30)."\n";
var_dump(gztell($h));
gzrewind($h);
var_dump(gztell($h));
echo "first 10 characters=".gzread($h, 10)."\n";
gzrewind($h);
echo "first 20 characters=".gzread($h, 20)."\n";
gzclose($h);
?>
--EXPECT--
test rewind before doing anything
bool(true)
int(0)

first 30 characters=When you're taught through fee
int(30)
int(0)
first 10 characters=When you'r
first 20 characters=When you're taught t
