--TEST--
False can be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(false $v = null) { return $v; }

var_dump(test(false));
var_dump(test(null));
?>
--EXPECT--
bool(false)
NULL
