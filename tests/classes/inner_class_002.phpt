--TEST--
inheritance of outer class
--FILE--
<?php

class Foo {
    public class Bar extends Foo {
        public function __construct() {
            parent::__construct();
            echo "Inner\n";
        }
    }
    public function __construct() {
        echo "Outer\n";
    }
}

$foo = new Foo::Bar();
?>
--EXPECT--
Outer
Inner
