--TEST--
gzcompress()/gzuncompress() and invalid params
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    var_dump(gzcompress("", 1000));
} catch (\Throwable $e) {
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
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gzuncompress("", -1));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gzuncompress(""));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gzuncompress("", 9));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzuncompress($data1));
var_dump(gzuncompress($data2));
$data2[4] = 0;
try {
    var_dump(gzuncompress($data2));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
gzcompress(): Argument #2 ($level) must be between -1 and 9
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
data error
gzuncompress(): Argument #2 ($max_length) must be greater than or equal to 0
data error
data error
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
data error
