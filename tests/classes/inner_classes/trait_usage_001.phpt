--TEST--
usage inside a trait
--FILE--
<?php

trait Outer {
    class Inner {}
}

var_dump(new Outer\Inner());

class Foo {
    use Outer;
}

var_dump(class_exists(Outer\Inner::class));
var_dump(class_exists(Foo\Inner::class));

?>
--EXPECTF--
Fatal error: Cannot declare class Outer\Inner inside a trait in %s on line %d
