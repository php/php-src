--TEST--
in_array() with references
--FILE--
<?php

$value = 42;
$array = [&$value];
var_dump(in_array(42, $array, false));
var_dump(in_array(42, $array, true));

?>
--EXPECT--
bool(true)
bool(true)
