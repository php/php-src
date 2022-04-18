--TEST--
Positional argument after named argument in new arguments
--FILE--
<?php

static $x = new stdClass(x: 0, 1);

?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
