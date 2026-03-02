--TEST--
Interface with __toString() method
--FILE--
<?php

interface MyStringable {
    public function __toString(): string;
}

$rc = new ReflectionClass(MyStringable::class);
var_dump($rc->getInterfaceNames());

?>
--EXPECT--
array(1) {
  [0]=>
  string(10) "Stringable"
}
