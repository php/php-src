--TEST--
Test function feof() by calling it with its expected arguments
--EXTENSIONS--
zlib
--FILE--
<?php
// note that gzeof is an alias to gzeof. parameter checking tests will be
// the same as gzeof

$f = __DIR__."/004.txt.gz";

echo "-- test 1 --\n";
$h = gzopen($f, 'r');
var_dump(gzeof($h));
gzpassthru($h);
var_dump(gzeof($h));
gzclose($h);

echo "\n-- test 2 --\n";
$h = gzopen($f, 'r');
echo "reading 50 characters. eof should be false\n";
gzread($h, 50)."\n";
var_dump(gzeof($h));
echo "reading 250 characters. eof should be true\n";
gzread($h, 250)."\n";
var_dump(gzeof($h));
echo "reading 20 characters. eof should be true still\n";
gzread($h, 20)."\n";
var_dump(gzeof($h));
gzclose($h);



?>
--EXPECT--
-- test 1 --
bool(false)
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
bool(true)

-- test 2 --
reading 50 characters. eof should be false
bool(false)
reading 250 characters. eof should be true
bool(true)
reading 20 characters. eof should be true still
bool(true)
