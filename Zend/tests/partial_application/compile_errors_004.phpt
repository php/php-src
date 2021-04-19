--TEST--
Partial application compile errors: named arguments must come after variadic place holder
--FILE--
<?php
foo(n: 5, ...);
?>
--EXPECTF--
Fatal error: Named arguments must come after all place holders in %s on line %d
