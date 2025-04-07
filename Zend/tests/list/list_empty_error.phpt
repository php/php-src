--TEST--
Empty list() assignments are not allowed
--FILE--
<?php

list(,,,,,,,,,,) = [];

?>
--EXPECTF--
Fatal error: Cannot use empty list in %s on line %d
