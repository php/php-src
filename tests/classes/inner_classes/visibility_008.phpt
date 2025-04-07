--TEST--
accessing sibling methods
--FILE--
<?php

class Outer {
    protected function test() {
        echo __METHOD__ . "\n";
    }
    protected class Middle {
        public static function test() {
            echo __METHOD__ . "\n";
        }
    }
}

class Other extends Outer {
    class Inner {
        public function test() {
            Outer\Middle::test();
            $t = new Outer();
            $t->test();
        }
    }
}
new Other\Inner()->test();
?>
--EXPECT--
Outer\Middle::test
Outer::test
