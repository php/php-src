--TEST--
False can be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(false $v = null) { return $v; }

var_dump(test(false));
var_dump(test(null));
?>
--EXPECTF--
Deprecated: test(): Implicitly marking parameter $v as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
bool(false)
NULL
