<?php

class Foo {
    public static function test() {
        return "foo::test";
    }
}

if (true) {
    function foo() {
        return 'I should be observable';
    }
}