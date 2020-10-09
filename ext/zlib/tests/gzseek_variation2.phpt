--TEST--
Test function gzseek() by calling it with SEEK_SET when reading
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

echo "move to the 50th byte\n";
var_dump(gzseek( $h, 50, SEEK_SET ) );
echo "tell=".gztell($h)."\n";
//read the next 10
var_dump(gzread($h, 10));

echo "\nmove forward to the 100th byte\n";
var_dump(gzseek( $h, 100, SEEK_SET ) );
echo "tell=".gztell($h)."\n";
//read the next 10
var_dump(gzread($h, 10));

echo "\nmove backward to the 20th byte\n";
var_dump(gzseek( $h, 20, SEEK_SET ) );
echo "tell=".gztell($h)."\n";
//read the next 10
var_dump(gzread($h, 10));
gzclose($h);
?>
--EXPECT--
move to the 50th byte
int(0)
tell=50
string(10) " high abov"

move forward to the 100th byte
int(0)
tell=100
string(10) "Destiny wh"

move backward to the 20th byte
int(0)
tell=20
string(10) "hrough fee"
