--TEST--
basic nested classes
--FILE--
<?php

namespace Foo;

class Outer {
    class Middle {
        class Inner {
            public function test() {
                echo __CLASS__;
            }
        }
    }
}
new Outer\Middle\Inner()->test();
?>
--EXPECT--
Foo\Outer\Middle\Inner
