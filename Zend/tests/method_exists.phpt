--TEST--
method_exists() segfaults
--FILE--
<?php
class testclass { function testfunc() { } }
var_dump(method_exists('testclass','testfunc'));
var_dump(method_exists('testclass','nonfunc'));
?>
--EXPECT--
bool(true)
bool(false)
