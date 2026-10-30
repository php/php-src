<?php
class Foo {
    public static function test() {
        static $i = 0;
        var_dump(++$i);
    }
}

Foo::test();
