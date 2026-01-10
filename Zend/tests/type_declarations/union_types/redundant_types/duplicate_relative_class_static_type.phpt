--TEST--
Duplicate static type
--FILE--
<?php

class Foo {
    public function method(array $data): static|static {}
}

?>
--EXPECTF--
Fatal error: Duplicate type static is redundant in %s on line %d
