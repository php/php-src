--TEST--
Error message for isset(func())
--FILE--
<?php
isset(abc());
?>
--EXPECTF--
Fatal error: Cannot use isset() on the result of a function call (you can use "null !== func()" instead) in %s on line %d
