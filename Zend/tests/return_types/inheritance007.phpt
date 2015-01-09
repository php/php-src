--TEST--
Inheritance Hinting Compile Checking Failure Internal Classes

--INI--
opcache.enable_cli=1

--FILE--
<?php
class Foo {
    public static function test() : Traversable {
        return new ArrayIterator([1, 2]);
    }
}

class Bar extends Foo {
    public static function test() : ArrayObject {
        return new ArrayObject([1, 2]);
    }
}

var_dump(Bar::test());
var_dump(Foo::test());

--EXPECTF--
Strict Standards: Declaration of Bar::test() should be compatible with Foo::test(): Traversable in %sinheritance007.php on line 12
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
object(ArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
