--TEST--
__set_state return type should support covariance
--FILE--
<?php

class Foo {
    public static function __set_state(array $data): self {}
}

class Foo2 {
    public static function __set_state(array $data): static {}
}

class Foo3 {
    public static function __set_state(array $data): Foo3|self {}
}

?>
===DONE===
--EXPECT--
===DONE===
