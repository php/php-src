--TEST--
Test invalid syntax of final properties
--FILE--
<?php

class Foo
{
    final final $property1;
}

?>
--EXPECTF--
Fatal error: Multiple final modifiers are not allowed in %S on line %d
