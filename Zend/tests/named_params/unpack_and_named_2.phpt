--TEST--
Named args before unpacking (not supported)
--FILE--
<?php

test(a: 42, ...[]);

?>
--EXPECTF--
Fatal error: Cannot use argument unpacking after named arguments in %s on line %d
