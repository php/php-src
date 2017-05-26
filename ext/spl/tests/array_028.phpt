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
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
