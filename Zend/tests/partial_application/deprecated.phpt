--TEST--
PFAs may emit deprecation warnings
--FILE--
<?php

#[Deprecated] function f($a) {
}

$f = f(?);
$f(1);

?>
--EXPECTF--
Deprecated: Function f() is deprecated in %s on line %d
