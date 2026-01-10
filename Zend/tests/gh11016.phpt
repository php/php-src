--TEST--
GH-11016 (Heap buffer overflow in ZEND_ADD_ARRAY_UNPACK_SPEC_HANDLER)
--FILE--
<?php
function number() {
    return 6;
}

// We need to use a function to trick the optimizer *not* to optimize the array to a constant
$x = [number() => 0, ...[1, 1, 1]];
print_r($x);
?>
--EXPECT--
Array
(
    [6] => 0
    [7] => 1
    [8] => 1
    [9] => 1
)
