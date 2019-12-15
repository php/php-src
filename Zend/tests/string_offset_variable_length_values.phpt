--TEST--
Some string offset replacement with variable length values
--FILE--
<?php

function str(): string { return "Hello world"; }
var_dump(str()[20] = '');
var_dump(str()[20] = 'あ');
var_dump(str()[0] = '');
var_dump(str()[0] = 'あ');
var_dump(str()[4] = '');
var_dump(str()[4] = 'あ');
var_dump(str()[-5] = '');
var_dump(str()[-5] = 'あ');

try {
    var_dump(str()[-20] = '');
} catch (\Error $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(str()[-20] = 'あ');
} catch (\Error $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
string(20) "Hello world         "
string(23) "Hello world         あ"
string(10) "ello world"
string(13) "あello world"
string(10) "Hell world"
string(13) "Hellあ world"
string(10) "Hello orld"
string(13) "Hello あorld"

Warning: Illegal string offset:  -20 in %s on line %d
NULL

Warning: Illegal string offset:  -20 in %s on line %d
NULL
