<?php

class A {
    public function test() {
        static $foo;
    }
}

class B extends A {}
