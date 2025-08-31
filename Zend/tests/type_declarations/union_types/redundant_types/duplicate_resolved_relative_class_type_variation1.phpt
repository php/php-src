--TEST--
Relative class type self resolving to an existing entry (after variation)
--FILE--
<?php

class Foo {
    public function method(array $data): Foo|self {}
}

?>
DONE
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
