--TEST--
substr_replace() function - array with unset
--FILE--
<?php

$replacement = ['A', 'C', 'B'];
unset($replacement[1]);
$newarr = substr_replace(['1 string', '2 string'], $replacement, 0);
print_r($newarr);

$replacement = ['foo', 42 => 'bar', 'baz'];
unset($replacement[42]);
$newarr = substr_replace(['1 string', '2 string'], $replacement, 0);
print_r($newarr);

?>
--EXPECT--
Array
(
    [0] => A
    [1] => B
)
Array
(
    [0] => foo
    [1] => baz
)
