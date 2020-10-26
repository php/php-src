--TEST--
Function redeclaration must produce a simple fatal
--FILE--
<?php
function f() {}
function f() {}
?>
--EXPECTF--
Fatal error: Function f() has already been declared (previous declaration in %s on line %d
