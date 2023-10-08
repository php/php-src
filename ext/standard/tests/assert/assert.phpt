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
--EXPECTF--
Deprecated: PHP Startup: assert.active INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_ACTIVE is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Deprecated: Constant ASSERT_WARNING is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
assertion failed 21,"assert($a != 0)"

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
class assertion failed 24,"assert($a != 0)"

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
class assertion failed 28,"assert($a != 0)"
