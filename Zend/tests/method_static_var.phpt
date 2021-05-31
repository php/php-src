--TEST--
Initial value of static var in method depends on the include time of the class definition
--FILE--
<?php

class Foo {
    public static function test() {
        static $i = 0;
        var_dump(++$i);
    }
}

Foo::test();
eval("class Bar extends Foo {}");
Foo::test();

Bar::test();
Bar::test();
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
