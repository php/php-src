--TEST--
Test invalid syntax of final constants
--FILE--
<?php

class Foo
{
    final const CONST1 = 1;
}

?>
--EXPECTF--
Fatal error: Cannot use 'final' as constant modifier in %s on line %d
