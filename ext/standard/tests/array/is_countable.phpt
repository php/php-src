--TEST--
Check if variables can be counted
--FILE--
<?php

$types = [
    "array"     =>  [],
    "null"      =>  null,
    "string"    =>  "test",
    "integer"   =>  123,
    "float"     =>  3.14,
    "true"      =>  true,
    "false"     =>  false,
    "object"    =>  (object) [],
    "simplexml" =>  new ArrayObject(["one", "two", "three"]),
    "countable" =>  new class implements Countable { function count() { return 1; }},
];

foreach ($types as $type => $value) {
    echo "{$type}: ";
    var_dump(is_countable($value));
}

?>
--EXPECTF--

array: bool(true)
null: bool(false)
string: bool(false)
integer: bool(false)
float: bool(false)
true: bool(false)
false: bool(false)
object: bool(false)
simplexml: bool(true)
countable: bool(true)
