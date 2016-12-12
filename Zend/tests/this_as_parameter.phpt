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
Fatal error: Cannot use $this as parameter in %sthis_as_parameter.php on line 2
