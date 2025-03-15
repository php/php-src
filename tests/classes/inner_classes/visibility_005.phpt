--TEST--
accessing outer private methods
--FILE--
<?php

class Outer {
    private function test() {
        echo __METHOD__ . "\n";
    }
    class Middle {
        private static function test() {
            echo __METHOD__ . "\n";
        }
        class Inner {
            public function test() {
                Outer:>Middle::test();
                $t = new Outer();
                $t->test();
            }
        }
    }
}
new Outer:>Middle:>Inner()->test();
?>
--EXPECT--
Outer:>Middle::test
Outer::test
