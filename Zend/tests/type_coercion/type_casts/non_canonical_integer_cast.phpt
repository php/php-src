--TEST--
Non canonical (integer) cast
--FILE--
<?php

var_dump((integer) "42");

?>
--EXPECTF--
Deprecated: Non-canonical cast (integer) is deprecated, use the (int) cast instead in %s on line %d
int(42)
