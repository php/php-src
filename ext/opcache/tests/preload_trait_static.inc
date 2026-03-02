<?php

trait Foo {
    public function test() {
        static $bar;
        var_dump($bar);
    }
}

class Bar {
    use Foo;
}
