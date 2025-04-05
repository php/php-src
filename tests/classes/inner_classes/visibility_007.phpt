--TEST--
accessing outer protected methods
--FILE--
<?php

class Outer {
    protected function test() {
        echo __METHOD__ . "\n";
    }
    class Middle {
        protected static function test() {
            echo __METHOD__ . "\n";
        }
        class Inner {
            public function test() {
                Middle::test();
                $t = new Outer();
                $t->test();
            }
        }
    }
}
new Outer\Middle\Inner()->test();
?>
--EXPECT--
Outer\Middle::test
Outer::test
