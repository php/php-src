--TEST--
Bug #34358 (Fatal error: Cannot re-assign $this(again))
--FILE--
<?php
class foo {
  function bar() {
    $ref = &$this;
  }
}
$x = new foo();
$x->bar();
echo "ok\n";
?>
--EXPECT--
ok
