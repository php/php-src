--TEST--
Enum cases increases refcount
--FILE--
<?php

enum Foo {
    case Bar;
}

function callCases() {
    Foo::cases();
}

callCases();
debug_zval_dump(Foo::Bar);

?>
--EXPECT--
object(Foo)#1 (1) refcount(2){
  ["name"]=>
  string(3) "Bar" interned
}
