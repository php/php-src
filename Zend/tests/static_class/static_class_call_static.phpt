--TEST--
Tests that a static class allows __callStatic() magic method
--FILE--
<?php

static class C {
    static function __callStatic(string $name, array $arguments) {
        echo $name;
    }
}

C::F();
?>
--EXPECT--
F
