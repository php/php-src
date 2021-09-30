--TEST--
False cannot be used as a standalone type even with implicit nullability
--FILE--
<?php

function test(false $v = null) {}

?>
--EXPECTF--
Fatal error: false cannot be used as a standalone type in %s on line %d
