--TEST--
The (real) cast is no longer supported
--FILE--
<?php

var_dump((real) 42);

?>
--EXPECTF--
Parse error: The (real) cast has been removed, use (float) instead in %s on line %d
