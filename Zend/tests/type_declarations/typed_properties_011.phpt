--TEST--
Test typed properties allow fetch reference for init array
--FILE--
<?php
class Foo {
    public int $bar = 1;
}

$foo = new Foo();

$array = [&$foo->bar];
var_dump($array);
?>
--EXPECT--
array(1) {
  [0]=>
  &int(1)
}
