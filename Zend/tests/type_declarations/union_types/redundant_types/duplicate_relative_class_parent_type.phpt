--TEST--
Duplicate parent type
--FILE--
<?php

class Foo {
    public function method(array $data) {}
}
class Bar extends Foo {
    public function method(array $data): parent|parent {}
}

?>
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
