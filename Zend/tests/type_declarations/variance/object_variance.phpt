--TEST--
Testing object's variance in inheritance
--FILE--
<?php

interface I1 {
  function method1(I1 $o): object;
}
interface I2 extends I1 {
  function method1(object $o): I1;
}
final class C1 implements I2 {
  function method1($o = null): self {
    return $this;
  }
}

$o = new C1();
echo get_class($o->method1());
?>
--EXPECT--
C1
