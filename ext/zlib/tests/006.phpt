--TEST--
gzdeflate()/gzinflate() and invalid params
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    var_dump(gzcompress("", 1000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzdeflate("", -1));

var_dump(gzdeflate(""));
var_dump(gzdeflate("", 9));

$string = "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony";

var_dump($data1 = gzdeflate($string));
var_dump($data2 = gzdeflate($string, 9));

var_dump(gzinflate(""));
var_dump(gzinflate("asfwe", 1000));

try {
    var_dump(gzinflate("asdf", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzinflate("asdf"));
var_dump(gzinflate("asdf", 9));

var_dump(gzinflate($data1));
var_dump(gzinflate($data2));
$data2[4] = 0;
var_dump(gzinflate($data2));

?>
--EXPECTF--
gzcompress(): Argument #2 ($level) must be between -1 and 9
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"

Warning: gzinflate(): data error in %s on line %d
bool(false)

Warning: gzinflate(): data error in %s on line %d
bool(false)
gzinflate(): Argument #2 ($max_length) must be greater than or equal to 0

Warning: gzinflate(): data error in %s on line %d
bool(false)

Warning: gzinflate(): data error in %s on line %d
bool(false)
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"

Warning: gzinflate(): data error in %s on line %d
bool(false)
