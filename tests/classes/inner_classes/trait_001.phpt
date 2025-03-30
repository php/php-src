--TEST--
nested traits
--FILE--
<?php

class Outer {
    trait Inner {
        public function foo() {
            return "foo";
        }
    }

    use Inner;
}

$outer = new Outer();
echo $outer->foo();
?>
--EXPECT--
foo
