--TEST--
list() with both keyed and unkeyed elements
--FILE--
<?php

$contrivedKeyedAndUnkeyedArrayExample = [
    0,
    1 => 1,
    "foo" => "bar"
];

list($zero, 1 => $one, "foo" => $foo) = $contrivedKeyedAndUnkeyedArrayExample;

?>
--EXPECTF--
Fatal error: Cannot mix array elements with and without keys in assignments in %s on line %d
