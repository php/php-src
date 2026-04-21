--TEST--
Final anonymous classes
--FILE--
<?php

$x = new final final class {};

?>
--EXPECTF--
Fatal error: Multiple final modifiers are not allowed in %s on line %d
