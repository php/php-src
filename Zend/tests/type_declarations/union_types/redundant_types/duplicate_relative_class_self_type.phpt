--TEST--
Duplicate self type
--FILE--
<?php

class Foo {
    public function method(array $data): self|self {}
}

?>
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
