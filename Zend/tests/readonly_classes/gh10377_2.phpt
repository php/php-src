--TEST--
GH-10377 (Unable to have an anonymous readonly class) - usage variation: abstract modifier
--FILE--
<?php

$x = new abstract class {};

?>
--EXPECTF--
Fatal error: Cannot use the abstract modifier on an anonymous class in %s on line %d
