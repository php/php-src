--TEST--
Error message for isset(func())
--FILE--
<?php
isset(1 + 1);
?>
--EXPECTF--
Fatal error: Cannot use isset() on the result of an expression (you can use "expression !== null" instead) in %s on line %d
