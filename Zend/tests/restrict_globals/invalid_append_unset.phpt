--TEST--
Cannot append to $GLOBALS in unset()
--FILE--
<?php
unset($GLOBALS[]);
?>
--EXPECTF--
Fatal error: Cannot use [] for unsetting in %s on line %d
