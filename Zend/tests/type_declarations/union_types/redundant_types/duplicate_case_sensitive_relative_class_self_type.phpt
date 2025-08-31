--TEST--
Duplicate self type in different cases
--FILE--
<?php

class Foo {
    public function method(array $data): self|SELF {}
}

?>
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
