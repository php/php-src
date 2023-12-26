--TEST--
Immediate method call on new object with ctor parentheses
--FILE--
<?php

class A
{
    public function test(): void
    {
        echo 'called';
    }
}

new A()->test();

?>
--EXPECT--
called
