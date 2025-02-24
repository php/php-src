--TEST--
instanceof
--FILE--
<?php

class Outer {
    public class Inner {
        public function __construct() {
            var_dump($this instanceof self);
        }
    }
}

$x = new Outer::Inner;
var_dump($x instanceof Outer::Inner);
?>
--EXPECT--
bool(true)
bool(true)
