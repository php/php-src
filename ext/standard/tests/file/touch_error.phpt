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
Warning: Wrong parameter count for touch() in %s on line %d
NULL

Warning: Wrong parameter count for touch() in %s on line %d
NULL

Warning: touch(): Unable to create file /no/such/file/or/directory because %s in %s on line %d
bool(false)