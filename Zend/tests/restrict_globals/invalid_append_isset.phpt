--TEST--
Cannot append to $GLOBALS in isset()
--FILE--
<?php
isset($GLOBALS[]);
?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
