--TEST--
Test that unsetting dynamic static properties of immutable classes is still not possible
--FILE--
<?php

immutable class Foo
{
}

try {
    unset(Foo::$property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Attempt to unset static property Foo::$property1
