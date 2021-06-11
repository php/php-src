--TEST--
Test function gzseek() by calling it with SEEK_CUR when reading
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');

echo "move to the 50th byte\n";
var_dump(gzseek( $h, 50, SEEK_CUR ) );
echo "tell=".gztell($h)."\n";
//read the next 10
var_dump(gzread($h, 10));

echo "\nmove forward to the 94th byte\n";
var_dump(gzseek( $h, 34, SEEK_CUR ) );
echo "tell=".gztell($h)."\n";
//read the next 10
var_dump(gzread($h, 10));

echo "\nmove backward to the 77th byte\n";
var_dump(gzseek( $h, -27, SEEK_CUR ) );
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

move forward to the 94th byte
int(0)
tell=94
string(10) "ze it
Dest"

move backward to the 77th byte
int(0)
tell=77
string(10) "hat you ca"
