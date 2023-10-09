--TEST--
Static in new is not supported
--FILE--
<?php

class Foo {
    public static function singleton() {
        static $x = new static;
        return $x;
    }
}

$x = Foo::singleton();
$y = Foo::singleton();
var_dump($x, $y);

?>
--EXPECT--
object(Foo)#1 (0) {
}
object(Foo)#1 (0) {
}
