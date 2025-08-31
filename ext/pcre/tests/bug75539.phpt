--TEST--
Bug #75539 - Recursive call errors are not reported by preg_last_error()
--SKIPIF--
<?php
if (10 == PCRE_VERSION_MAJOR && 37 == PCRE_VERSION_MINOR) {
    die("skip ");
}
?>
--FILE--
<?php

var_dump(preg_match('/((?1)?z)/', ''));
var_dump(preg_last_error() === \PREG_INTERNAL_ERROR);

?>
--EXPECT--
int(0)
bool(false)
