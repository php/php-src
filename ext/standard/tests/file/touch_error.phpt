--TEST--
touch() error tests
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

var_dump(touch("/no/such/file/or/directory"));

?>
--EXPECTF--
Warning: touch(): Unable to create file /no/such/file/or/directory because No such file or directory in %s on line %d
bool(false)
