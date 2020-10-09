--TEST--
Test preg_match() function : error conditions - Internal error
--FILE--
<?php

var_dump(preg_match('/', 'Hello world'));
var_dump(preg_last_error_msg() === 'Internal error');

?>
--EXPECTF--
Warning: preg_match(): No ending delimiter '/' found in %s on line %d
bool(false)
bool(true)
