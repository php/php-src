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
Fatal error: Cannot use $this as parameter in %s on line %d
