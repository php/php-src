--TEST--
usage in an enum
--FILE--
<?php

enum Outer {
    class Inner {}
}

var_dump(new Outer\Inner());
var_dump(class_exists(Outer\Inner::class));
?>
--EXPECT--
object(Outer\Inner)#1 (0) {
}
bool(true)
