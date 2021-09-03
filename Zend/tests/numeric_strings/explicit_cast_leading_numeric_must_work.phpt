--TEST--
Explicit cast of leading numeric strings should still work without warning
--FILE--
<?php

var_dump((int) "2px");
var_dump((float) "2px");
var_dump((int) "2.5px");
var_dump((float) "2.5px");

?>
--EXPECT--
int(2)
float(2)
int(2)
float(2.5)
