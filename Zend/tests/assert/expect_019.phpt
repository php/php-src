--TEST--
test assertions in namespace (assertions completely disabled)
--INI--
zend.assertions=-1
assert.exception=0
--FILE--
<?php
namespace Foo;

var_dump(\assert(false));
var_dump(\assert(true));
var_dump(assert(false));
var_dump(assert(true));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
