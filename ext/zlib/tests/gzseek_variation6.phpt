--TEST--
Test function gzseek() by calling it with SEEK_END when reading
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
// move 40 bytes
echo "move 40 bytes\n";
gzread($h, 40);
echo "tell=";
var_dump(gztell($h));
echo "move to the end\n";
try {
    var_dump(gzseek( $h, 0, SEEK_END ) );
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "tell=";
var_dump(gztell($h));
echo "eof=";
var_dump(gzeof($h));
//read the next 10
var_dump(gzread($h, 10));
gzclose($h);
?>
--EXPECTF--
move 40 bytes
tell=int(40)
move to the end
SEEK_END is not supported
tell=int(40)
eof=bool(false)
string(10) "iny flying"
