--TEST--
Relative class type self resolving to an existing entry (before variation)
--FILE--
<?php

class Foo {
    public function method(array $data): self|Foo {}
}

?>
--EXPECTF--
Fatal error: self resolves to Foo which is redundant in %s on line %d
