--TEST--
Duplicate parent type in different cases
--FILE--
<?php

class Foo {
    public function method(array $data) {}
}
class Bar extends Foo {
    public function method(array $data): parent|PARENT {}
}

?>
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
