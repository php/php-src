--TEST--
Test unserialize() with extra data at the end of a valid value with nested unserialize
--FILE--
<?php

final class Foo {
    public $foo;

    public function __unserialize(array $foo)
    {
        $this->foo = unserialize($foo['bar']);
    }

    public function __serialize(): array
    {
        return [
            'bar' => serialize($this->foo) . 'garbage',
        ];
    }
}

$f = new Foo;
$f->foo = ['a', 'b', 'c'];

var_dump(unserialize(serialize($f) . 'garbage'));

?>
--EXPECTF--
Warning: unserialize(): Extra data starting at offset 81 of 88 bytes in %s on line %d

Warning: unserialize(): Extra data starting at offset 42 of 49 bytes in %s on line %d
object(Foo)#2 (1) {
  ["foo"]=>
  array(3) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
    [2]=>
    string(1) "c"
  }
}
