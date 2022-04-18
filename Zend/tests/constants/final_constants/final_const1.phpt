--TEST--
Class constants support the final modifier
--FILE--
<?php

class Foo
{
    final const A = "foo";
    final public const B = "foo";
}

?>
--EXPECT--
