--TEST--
#49274, fatal error when an object does not implement toString
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip filter extension not loaded"); ?>
--FILE--
<?php
var_dump(filter_var(new stdClass, FILTER_VALIDATE_EMAIL));
?>
--EXPECT--	
bool(false)
