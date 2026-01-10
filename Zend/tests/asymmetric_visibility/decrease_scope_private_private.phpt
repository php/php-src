--TEST--
Asymmetric visibility private(get) private(set) is allowed
--FILE--
<?php

class Foo {
    private private(set) string $bar;
}

?>
===DONE===
--EXPECT--
===DONE===
