--TEST--
Argument unpacking in new arguments in const expr (not yet supported)
--FILE--
<?php

static $x = new stdClass(...[0]);

?>
--EXPECTF--
Fatal error: Argument unpacking in constant expressions is not supported in %s on line %d
