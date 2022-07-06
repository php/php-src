--TEST--
globals in local scope - 3
--INI--
variables_order="egpcs"
--FILE--
<?php

function test() {
    include __DIR__."/globals.inc";
}

test();

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(false)
string(5) "array"
int(%d)
string(%d) "%s"

Warning: Undefined array key "PHP_SELF" in %s on line %d
NULL

Warning: Undefined variable $_SERVER in %s on line %d
NULL
Done
