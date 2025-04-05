--TEST--
shadowing
--FILE--
<?php

namespace Foo\Bar;

class Outside {}

class Outer {
    class Outside {}
    class Inner extends Outside {}
    class Middle extends \Foo\Bar\Outside {}
}

echo Outer\Inner::class . ': ' . (new Outer\Inner()) instanceof Outer\Outside . "\n";
echo Outer\Middle::class . ': ' . (new Outer\Middle()) instanceof Outside . "\n";
?>
--EXPECT--
Foo\Bar\Outer\Inner: 1
Foo\Bar\Outer\Middle: 1
