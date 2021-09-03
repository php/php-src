<?php
class Foo {
    public static function test() {
        static $i = 0;
        var_dump(++$i);
    }
}


Foo::test();
eval("class Bar extends Foo {}"); // Avoid early binding.
Foo::test();
Bar::test();
Bar::test();
echo "\n";
