--TEST--
testing the behavior of string offsets
--FILE--
<?php
$string = "foobar";
const FOO = "BAR"[0];
var_dump(FOO);
var_dump($string[0]);
var_dump($string[1]);
var_dump(isset($string[0]));
var_dump(isset($string[0][0]));
try {
    var_dump($string["foo"]);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(isset($string["foo"]["bar"]));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
string(1) "B"
string(1) "f"
string(1) "o"
bool(true)
bool(true)
Cannot access offset of type string on string

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(false)
