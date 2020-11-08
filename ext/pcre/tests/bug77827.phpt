--TEST--
Bug #77827 (preg_match does not ignore \r in regex flags)
--FILE--
<?php
var_dump(
    preg_match("/foo/i\r", 'FOO'),
    preg_last_error()
);
?>
--EXPECT--
int(1)
int(0)
