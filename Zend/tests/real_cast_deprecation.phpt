--TEST--
The (real) cast is deprecated
--FILE--
<?php

var_dump((real) 42);

?>
--EXPECTF--
Deprecated: The (real) cast is deprecated, use (float) instead in %s on line %d
float(42)
