--TEST--
assert()
--INI--
assert.active = 0
assert.warning = 1
assert.callback =
assert.bail = 0
assert.exception=0
--FILE--
<?php
function a($file, $line, $unused, $desc)
{
    echo "assertion failed $line,\"$desc\"\n";
}

class a
{
    static function assert($file, $line, $unused, $desc)
    {
        echo "class assertion failed $line,\"$desc\"\n";
    }
}

assert_options(ASSERT_ACTIVE,1);
assert_options(ASSERT_WARNING,0);

$a = 0;

assert_options(ASSERT_CALLBACK,"a");
assert($a != 0);

assert_options(ASSERT_CALLBACK,array("a","assert"));
assert($a != 0);

$obj = new a();
assert_options(ASSERT_CALLBACK,array(&$obj,"assert"));
assert($a != 0);
?>
--EXPECT--
assertion failed 21,"assert($a != 0)"
class assertion failed 24,"assert($a != 0)"
class assertion failed 28,"assert($a != 0)"
