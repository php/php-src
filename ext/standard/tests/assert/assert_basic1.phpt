--TEST--
assert() - basic - check  that assert can be switched off
--INI--
assert.active = 0
assert.warning = 0
assert.callback = f1
assert.bail = 0
--FILE--
<?php
function f1()
{
    echo "f1 called\n";
}

var_dump($r2=assert(0));
var_dump($r2=assert(1));
?>
--EXPECT--
bool(true)
bool(true)
