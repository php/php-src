--TEST--
scope doesn't bypass scope
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
            public function testit() {
                $this->test();
            }
        }
    }
}
new Outer\Middle\Inner()->testit();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method Outer\Middle\Inner::test() in %s:%d
Stack trace:
#0 %s(%d): Outer\Middle\Inner->testit()
#1 {main}
  thrown in %s on line %d
