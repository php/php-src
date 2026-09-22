--TEST--
GH-10377 (Unable to have an anonymous readonly class) - usage variation: multiple readonly modifiers
--FILE--
<?php

$x = new readonly readonly class {};

?>
--EXPECTF--
Fatal error: Multiple readonly modifiers are not allowed in %s on line %d
