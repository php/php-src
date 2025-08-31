--TEST--
Traits and get_called_class().
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

    trait TestTrait {
        public static function test() {
            return get_called_class();
        }
    }

    class A {
        use TestTrait;
    }

    class B extends A { }

    echo B::test();

?>
--EXPECT--
B
