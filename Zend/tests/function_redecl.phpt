--TEST--
Function redeclaration must produce a simple fatal
--FILE--
<?php
function f() {}
function f() {}
?>
--EXPECTF--
Fatal error: Function f() cannot be redeclared (previous declaration in %s on line %d
