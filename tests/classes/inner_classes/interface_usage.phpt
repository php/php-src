--TEST--
usage in an interface
--FILE--
<?php

interface Outer {
    class Inner {}
}

var_dump(new Outer\Inner());

class Foo implements Outer {}

var_dump(class_exists(Outer\Inner::class));
var_dump(class_exists(Foo\Inner::class));
?>
--EXPECT--
object(Outer\Inner)#1 (0) {
}
bool(true)
bool(false)
