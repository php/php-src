--TEST--
Traits with static methods referenced using variable.
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

    $class = "A";
    echo $class::test();

?>
--EXPECT--
Test
