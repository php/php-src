--TEST--
$this as parameter
--FILE--
<?php
function foo($this) {
    var_dump($this);
}
foo(5);
?>
--EXPECTF--
Fatal error: foo(): Parameter #1 cannot be called $this in %sthis_as_parameter.php on line 2
