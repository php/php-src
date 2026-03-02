--TEST--
Test unserialize() with extra data at the end of a valid value with Serializable
--FILE--
<?php

final class Foo implements Serializable {
    public $foo;

    public function unserialize(string $foo)
    {
        $this->foo = unserialize($foo);
    }

    public function serialize(): string
    {
        return serialize($this->foo) . 'garbage';
    }
}

$f = new Foo;
$f->foo = ['a', 'b', 'c'];

var_dump(unserialize(serialize($f) . 'garbage'));

?>
--EXPECTF--
Deprecated: Foo implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: unserialize(): Extra data starting at offset 42 of 49 bytes in %s on line %d

Warning: unserialize(): Extra data starting at offset 64 of 71 bytes in %s on line %d
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
