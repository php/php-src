--TEST--
gzcompress()/gzuncompress() and invalid params
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gzcompress("", 1000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzcompress("", -1));
var_dump(gzcompress(""));
var_dump(gzcompress("", 9));

$string = "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony";

var_dump($data1 = gzcompress($string));
var_dump($data2 = gzcompress($string, 9));

try {
    var_dump(gzuncompress("", 1000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gzuncompress("", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzuncompress(""));
var_dump(gzuncompress("", 9));

var_dump(gzuncompress($data1));
var_dump(gzuncompress($data2));
$data2[4] = 0;
var_dump(gzuncompress($data2));

?>
--EXPECTF--
gzcompress(): Argument #2 ($level) must be between -1 and 9
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"

Warning: gzuncompress(): %s error in %s on line %d
bool(false)
gzuncompress(): Argument #2 ($max_length) must be greater than or equal to 0

Warning: gzuncompress(): %s error in %s on line %d
bool(false)

Warning: gzuncompress(): %s error in %s on line %d
bool(false)
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"

Warning: gzuncompress(): %s error in %s on line %d
bool(false)
