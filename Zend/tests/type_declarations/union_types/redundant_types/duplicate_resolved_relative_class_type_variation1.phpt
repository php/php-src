--TEST--
Relative class type self resolving to an existing entry (after variation)
--FILE--
<?php

class Foo {
    public function method(array $data): Foo|self {}
}

?>
DONE
--EXPECT--
DONE
