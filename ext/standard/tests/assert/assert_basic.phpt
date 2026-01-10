--TEST--
assert() - basic - check  that assert runs when assert.active is set to 1.
--INI--
assert.active = 1
assert.warning = 0
assert.callback = f1
assert.bail = 0
assert.exception=0
--FILE--
<?php
function f1()
{
    echo "f1 called\n";
}

var_dump($r2 = assert(0));
var_dump($r2 = assert(1));
?>
--EXPECTF--
Deprecated: PHP Startup: assert.warning INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.callback INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0
f1 called
bool(false)
bool(true)
