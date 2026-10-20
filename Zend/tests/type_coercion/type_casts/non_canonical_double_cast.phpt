--TEST--
Non canonical (double) cast
--FILE--
<?php

var_dump((double) 42);

?>
--EXPECTF--
Deprecated: Non-canonical cast (double) is deprecated, use the (float) cast instead in %s on line %d
float(42)
