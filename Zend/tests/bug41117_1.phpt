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
Fatal error: Cannot re-assign $this in %sbug41117_1.php on line 3

