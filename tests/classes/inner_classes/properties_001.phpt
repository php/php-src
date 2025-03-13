--TEST--
property types
--FILE--
<?php

class Outer {
    public self:>Inner $inner;
    private class Inner {}
    public function test(): void {
        $this->inner = new self:>Inner();
    }
}

$outer = new Outer();
$outer->test();
var_dump($outer->inner);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Method test is public but returns a private class: Outer:>Inner in %s:%d
Stack trace:
#0 %s(%d): Outer::test()
#1 {main}
  thrown in %s on line %d
