--TEST--
Test to check regressions on use statements and lexer state
--FILE--
<?php

use A\B\C\D;

class Foo
{
    private static $foo;

}

echo PHP_EOL, "Done", PHP_EOL;
--EXPECT--
Done
