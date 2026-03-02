--TEST--
Immediate method call on new object without constructor parentheses
--FILE--
<?php

class A
{
    public function test(): void
    {
        echo 'called';
    }
}

new A->test();

?>
--EXPECTF--
Parse error: syntax error, unexpected token "->" in %s on line %d
