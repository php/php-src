--TEST--
assert() - basic - correct call back values before and after assert.
--INI--
assert.active = 1
assert.warning = 1
assert.callback=f1
assert.bail = 0
assert.exception=0
--FILE--
<?php
function f2()
{
    echo "f2 called\n";
}

function f1()
{
    echo "f1 called\n";
}

var_dump($o = assert_options(ASSERT_CALLBACK));
assert(0);

var_dump($o= assert_options(ASSERT_CALLBACK, "f2"));
var_dump($n= assert_options(ASSERT_CALLBACK));
assert(0);
?>
--EXPECTF--
Deprecated: PHP Startup: assert.callback INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
string(2) "f1"
f1 called

Warning: assert(): assert(0) failed in %s on line %d

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
string(2) "f1"

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
string(2) "f2"
f2 called

Warning: assert(): assert(0) failed in %s on line %d
