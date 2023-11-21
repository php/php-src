--TEST--
Bug #69889: Null coalesce operator doesn't work for string offsets
--FILE--
<?php

$foo = "test";
var_dump($foo[0] ?? "default");

var_dump($foo[5] ?? "default");
var_dump(isset($foo[5]) ? $foo[5] : "default");

try {
    var_dump($foo["str"] ?? "default");
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    var_dump(isset($foo["str"]) ? $foo["str"] : "default");
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
string(1) "t"
string(7) "default"
string(7) "default"

Warning: Cannot access offset of type string in isset or empty in %s on line %d
string(7) "default"

Warning: Cannot access offset of type string in isset or empty in %s on line %d
string(7) "default"
