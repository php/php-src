--TEST--
PFA preserves #[NoDiscard]
--FILE--
<?php

#[NoDiscard] function f($a) {
}

$f = f(?);
$f(1);

(void) $f(1);

?>
--EXPECTF--
Warning: The return value of function {closure:%s}() should either be used or intentionally ignored by casting it as (void) in %s on line 7
