--TEST--
touch() error tests
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

var_dump(touch());
var_dump(touch(1, 2, 3, 4));
var_dump(touch("/no/such/file/or/directory"));

?>
--EXPECTF--	
Warning: touch() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: touch() expects at most 3 parameters, 4 given in %s on line %d
NULL

Warning: touch(): Unable to create file /no/such/file/or/directory because No such file or directory in %s on line %d
bool(false)

