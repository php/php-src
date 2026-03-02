--TEST--
Property constant expression lhs wrong type
--FILE--
<?php

const A_prop = (42)->prop;
var_dump(A_prop);

const A_prop_nullsafe = (42)?->prop;
var_dump(A_prop_nullsafe);

?>
--EXPECTF--
Warning: Attempt to read property "prop" on int in %s on line %d
NULL

Warning: Attempt to read property "prop" on int in %s on line %d
NULL
