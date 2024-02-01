--TEST--
true can be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(true $v = null) { return $v; }

var_dump(test(true));
var_dump(test(null));
?>
--EXPECT--
bool(true)
NULL
