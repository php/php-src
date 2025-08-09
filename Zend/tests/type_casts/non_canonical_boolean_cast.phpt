--TEST--
Non canonical (boolean) cast
--FILE--
<?php

var_dump((boolean) 42);

?>
--EXPECTF--
Deprecated: Non-canonical cast (boolean) is deprecated, use the (bool) cast instead in %s on line %d
bool(true)
