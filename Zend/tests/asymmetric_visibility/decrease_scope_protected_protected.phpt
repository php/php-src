--TEST--
Asymmetric visibility protected(get) protected(set) is allowed
--FILE--
<?php

class Foo {
    protected protected(set) string $bar;
}

?>
===DONE===
--EXPECT--
===DONE===
