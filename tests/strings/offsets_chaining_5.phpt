--TEST--
testing the behavior of string offset chaining
--FILE--
<?php
$array = array('expected_array' => "foobar");
var_dump(isset($array['expected_array']));
var_dump($array['expected_array']);
try {
    var_dump(isset($array['expected_array']['foo']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
var_dump($array['expected_array']['0foo']);
try {
    var_dump(isset($array['expected_array']['foo']['bar']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
var_dump($array['expected_array']['0foo']['0bar']);
?>
--EXPECTF--
bool(true)
string(6) "foobar"
Cannot access offset of type string on string

Warning: Illegal string offset "0foo" in %s on line %d
string(1) "f"
Cannot access offset of type string on string

Warning: Illegal string offset "0foo" in %s on line %d

Warning: Illegal string offset "0bar" in %s on line %d
string(1) "f"
