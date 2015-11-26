--TEST--
Positional arguments cannot be used after argument unpacking
--FILE--
<?php

var_dump(...[1, 2, 3], 4);

?>
--EXPECTF--
Fatal error: Cannot use positional argument after argument unpacking in %s on line %d
