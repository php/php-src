--TEST--
Test serialize_precision (part 2)
--INI--
serialize_precision=75
--FILE--
<?php
var_dump(serialize(0.1));
?>
--EXPECT--
string(60) "d:0.1000000000000000055511151231257827021181583404541015625;"
