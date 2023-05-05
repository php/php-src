--TEST--
GH-10377 (Unable to have an anonymous readonly class) - usage variation: final modifier
--FILE--
<?php

$x = new final class {};

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an anonymous class in %s on line %d
