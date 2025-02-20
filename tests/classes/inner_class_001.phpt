--TEST--
a simple inner class
--FILE--
<?php

class Foo {
    public class Bar {
        public function __construct() {
            echo "Inner\n";
        }
    }
}

var_dump(Foo::Bar::class);
?>
--EXPECT--
string(8) "Foo::Bar"
