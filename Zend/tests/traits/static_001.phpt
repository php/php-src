--TEST--
Traits with static methods.
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

    trait TestTrait {
        public static function test() {
            return 'Test';
        }
    }

    class A {
        use TestTrait;
    }

    echo A::test();

?>
--EXPECT--
Test
