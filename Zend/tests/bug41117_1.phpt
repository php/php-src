--TEST--
Bug #41117 (Altering $this via argument)
--FILE--
<?php
class foo {
  function __construct($this) {
    echo $this."\n";
  }
}
$obj = new foo("Hello world");
?>
--EXPECTF--
Fatal error: foo::__construct(): Parameter #1 cannot be called $this in %s on line %d
