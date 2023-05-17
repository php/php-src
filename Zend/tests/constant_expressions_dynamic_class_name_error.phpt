--TEST--
Dynamic class names can't be used in compile-time constant refs
--FILE--
<?php

$foo = 'test';
const C = $foo::BAR;

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line %d
