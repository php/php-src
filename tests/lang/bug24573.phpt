--TEST--
Bug #24573 (debug_backtrace() crashes if $this is set to null)
--FILE--
<?php

class Foo {
  function Bar() {
    $__this = $this;
    $this = null;
    debug_backtrace();
    $this = $__this;
  }
}

$f = new Foo;

$f->Bar();

echo "OK\n";

?>
--EXPECTF--

Fatal error: Cannot re-assign $this in %s on line %d
