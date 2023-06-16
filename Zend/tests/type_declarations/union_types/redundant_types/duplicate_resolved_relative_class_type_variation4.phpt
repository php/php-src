--TEST--
Relative class type parent resolving to an existing entry (before variation)
--FILE--
<?php

class Foo {
    public function method(array $data) {}
}
class Bar extends Foo {
    public function method(array $data): parent|Foo {}
}

?>
--EXPECTF--
Fatal error: parent resolves to Foo which is redundant in %s on line %d
