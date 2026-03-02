--TEST--
Test serialize_precision (part 1)
--INI--
serialize_precision=10
--FILE--
<?php
var_dump(serialize(0.1));
?>
--EXPECT--
string(6) "d:0.1;"
