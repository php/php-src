--TEST--
ArrayObject/Iterator on array with NUL bytes
--FILE--
<?php
$array = [
    "\0foo" => "bar",
];

$it = new ArrayIterator($array);
foreach ($it as $v) {
    var_dump($v);
}

$obj = new ArrayObject($array);
foreach ($obj as $v) {
    var_dump($v);
}

$obj = new ArrayObject($it);
foreach ($obj as $v) {
    var_dump($v);
}
?>
--EXPECTF--
string(3) "bar"
string(3) "bar"

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
string(3) "bar"
