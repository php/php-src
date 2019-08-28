--TEST--
Operator precedence
--FILE--
<?php

var_dump((object)1 instanceof StdClass);
var_dump(! (object)1 instanceof Exception);

?>
--EXPECT--
bool(true)
bool(true)
