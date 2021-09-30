--TEST--
Nullable false cannot be used as a standalone type
--FILE--
<?php

function test(): ?false {}

?>
--EXPECTF--
Fatal error: false cannot be marked as nullable since false is not a standalone type in %s on line %d
