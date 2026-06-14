--TEST--
GH-11320: list() must not appear as a standalone array element
--FILE--
<?php
[list($a)];
?>
--EXPECTF--
Fatal error: Cannot use list() as standalone expression in %s on line %d
