--TEST--
$this in unset
--FILE--
<?php
unset($this);
?>
--EXPECTF--
Fatal error: Cannot unset $this in %sthis_in_unset.php on line 2
