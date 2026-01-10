--TEST--
Relative class type parent resolving to an existing entry (after variation)
--FILE--
<?php

class Foo {
    public function method(array $data) {}
}
class Bar extends Foo {
    public function method(array $data): Foo|parent {}
}

?>
DONE
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
