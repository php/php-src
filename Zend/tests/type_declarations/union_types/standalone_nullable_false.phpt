--TEST--
Nullable false cannot be used as a standalone type
--FILE--
<?php

function test(): ?false {}

?>
--EXPECTF--
Fatal error: False cannot be used as a standalone type in %s on line %d
