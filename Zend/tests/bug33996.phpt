--TEST--
Bug #33996 (No information given for fatal error on passing invalid value to typed argument)
--INI--
error_reporting=8191
--FILE--
<?php
class Foo
{
    // nothing
}

function FooTest(Foo $foo)
{
    echo "Hello!";
}

function NormalTest($a)
{
    echo "Hi!";
}

NormalTest();
FooTest();
FooTest(new Foo());
?>
--EXPECTF--
Warning: Missing argument 1 for NormalTest(), called in %sbug33996.php on line %d and defined in %sbug33996.php on line %d
Hi!
Fatal error: Argument 1 passed to FooTest() must be an instance of Foo, none given, called in %sbug33996.php on line %d and defined in %sbug33996.php on line %d
