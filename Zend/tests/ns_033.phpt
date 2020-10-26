--TEST--
033: Import statement with non-compound name
--FILE--
<?php
use A;
use \B;
?>
--EXPECTF--
Warning: Importing the global symbol A has no effect in the global scope in %s on line %d

Warning: Importing the global symbol B has no effect in the global scope in %s on line %d
