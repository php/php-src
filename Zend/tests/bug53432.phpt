--TEST--
Bug #53432: Assignment via string index access on an empty string converts to array
--FILE--
<?php

$str = '';
var_dump($str[0] = 'a');
var_dump($str);

$str = '';
var_dump($str[5] = 'a');
var_dump($str);

$str = '';
var_dump($str[-1] = 'a');
var_dump($str);

$str = '';
try {
    var_dump($str['foo'] = 'a');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($str);

$str = '';
try {
    var_dump($str[] = 'a');
} catch (Error $e) {
    echo "Error: {$e->getMessage()}\n";
}
var_dump($str);

$str = '';
try {
    var_dump($str[0] += 1);
} catch (Error $e) {
    echo "Error: {$e->getMessage()}\n";
}
var_dump($str);

$str = '';
try {
    var_dump($str[0][0] = 'a');
} catch (Error $e) {
    echo "Error: {$e->getMessage()}\n";
}
var_dump($str);

?>
--EXPECTF--
string(1) "a"
string(1) "a"
string(1) "a"
string(6) "     a"

Warning: Illegal string offset -1 in %s on line %d
NULL
string(0) ""
Cannot access offset of type string on string
string(0) ""
Error: [] operator not supported for strings
string(0) ""
Error: Cannot use assign-op operators with string offsets
string(0) ""
Error: Cannot use string offset as an array
string(0) ""
