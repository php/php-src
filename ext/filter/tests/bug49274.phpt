--TEST--
#49274, fatal error when an object does not implement toString
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var(new stdClass, FILTER_VALIDATE_EMAIL));
?>
--EXPECT--
bool(false)
