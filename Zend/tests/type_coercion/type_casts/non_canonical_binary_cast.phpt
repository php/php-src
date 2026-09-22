--TEST--
Non canonical (binary) cast
--FILE--
<?php

var_dump((binary) 42);

?>
--EXPECTF--
Deprecated: Non-canonical cast (binary) is deprecated, use the (string) cast instead in %s on line %d
string(2) "42"
