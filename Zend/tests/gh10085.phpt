--TEST--
GH-10085: Assertion in add_function_array()
--FILE--
<?php
$i = [[], 0];
$ref = &$i;
$i[0] += $ref;
?>
--EXPECT--
