--TEST--
assert() - set callback using ini_set()
--INI--
assert.active = 1
assert.warning = 0
assert.callback =
assert.bail = 0
assert.exception=0
--FILE--
<?php
function a($file, $line, $unused, $desc)
{
            echo "assertion failed - a - $line,\"$desc\"\n";
}

function b($file, $line, $unused, $desc)
{
            echo "assertion failed - b - $line,\"$desc\"\n";
}

assert_options(ASSERT_ACTIVE,1);
assert_options(ASSERT_WARNING,0);

$a = 0;

assert_options(ASSERT_CALLBACK, "a");
assert($a != 0);

 /* Modify call back using ini_set() */
ini_set("assert.callback", "b");
assert($a != 0);

?>
--EXPECT--
assertion failed - a - 18,"assert($a != 0)"
assertion failed - b - 22,"assert($a != 0)"
