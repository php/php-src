--TEST--
Dynamic class name in new is not supported
--FILE--
<?php

const FOO = new (BAR);

?>
--EXPECTF--
Fatal error: Cannot use dynamic class name in constant expression in %s on line %d
