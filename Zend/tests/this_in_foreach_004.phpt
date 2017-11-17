--TEST--
$this in foreach
--FILE--
<?php
$a = [[1]];
foreach ($a as list($this)) {
	var_dump($this);
}
?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_004.php on line 3
