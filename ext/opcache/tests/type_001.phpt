--TEST--
Type persistene 001
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
  class Foo {
  }
  class y extends Foo {
      public (y&A)|X $y;
  }
}
foo();
?>
DONE
--EXPECT--
DONE
