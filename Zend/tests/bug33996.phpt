--TEST--
Bug #33996 (No information given for fatal error on passing invalid value to typed argument)
--INI--
error_reporting=4095
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
Warning: Missing argument 1 for NormalTest(), called in %sbug33996.php on line 17 and defined in %sbug33996.php on line 12
Hi!
Fatal error: Argument 1 passed to FooTest() must be an object of class Foo, called in %sbug33996.php on line 18 and defined in %sbug33996.php on line 7
