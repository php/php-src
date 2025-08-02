--TEST--
Function redeclaration must produce a simple fatal
--FILE--
<?php
function f() {}
function f() {}
?>
--EXPECTF--
Fatal error: Cannot redeclare function f() (previously declared in %s:%d) in %s on line %d
