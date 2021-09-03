--TEST--
Test function gzread() by calling it invalid lengths
--EXTENSIONS--
zlib
--FILE--
<?php
$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
var_dump(gzread($h, 10));
try {
    var_dump(gzread($h, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gzread($h, 5));
try {
    var_dump(gzread($h, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gzread($h, 8));
gzclose($h);

?>
--EXPECT--
string(10) "When you'r"
gzread(): Argument #2 ($length) must be greater than 0
string(5) "e tau"
gzread(): Argument #2 ($length) must be greater than 0
string(8) "ght thro"
