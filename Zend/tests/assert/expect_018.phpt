--TEST--
test assertions in namespace
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php
namespace Foo;

ini_set("zend.assertions", 0);
var_dump(\assert(false));
var_dump(\assert(true));
var_dump(assert(false));
var_dump(assert(true));
ini_set("zend.assertions", 1);
var_dump(\assert(false));
var_dump(\assert(true));
var_dump(assert(false));
var_dump(assert(true));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)

Warning: assert(): assert(false) failed in %sexpect_018.php on line 10
bool(false)
bool(true)

Warning: assert(): assert(false) failed in %sexpect_018.php on line 12
bool(false)
bool(true)
