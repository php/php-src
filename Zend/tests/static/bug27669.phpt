--TEST--
Bug #27669 (PHP 5 didn't support all possibilities for calling static methods dynamically)
--FILE--
<?php
    class A {
        static function hello() {
            echo "Hello World\n";
        }
    }
    $y[0] = 'hello';
    A::{$y[0]}();
?>
--EXPECT--
Hello World
