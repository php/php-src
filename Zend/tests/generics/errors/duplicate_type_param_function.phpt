--TEST--
Errors: duplicate type parameter name on function
--FILE--
<?php
function f<U, U>(): void {}
?>
--EXPECTF--
Fatal error: Cannot redeclare type parameter U in %s on line %d
