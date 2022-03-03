--TEST--
Enum unserialize refcount
--FILE--
<?php

enum Foo {
    case Bar;
}

debug_zval_dump(Foo::Bar);

$foo = Foo::Bar;
debug_zval_dump($foo);

$bar = unserialize('E:7:"Foo:Bar";');
debug_zval_dump($foo);

unset($bar);
debug_zval_dump($foo);

unset($foo);
debug_zval_dump(Foo::Bar);

?>
--EXPECT--
object(Foo)#1 (1) refcount(2){
  ["name"]=>
  string(3) "Bar" interned
}
object(Foo)#1 (1) refcount(3){
  ["name"]=>
  string(3) "Bar" interned
}
object(Foo)#1 (1) refcount(4){
  ["name"]=>
  string(3) "Bar" interned
}
object(Foo)#1 (1) refcount(3){
  ["name"]=>
  string(3) "Bar" interned
}
object(Foo)#1 (1) refcount(2){
  ["name"]=>
  string(3) "Bar" interned
}
