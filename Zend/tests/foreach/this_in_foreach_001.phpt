--TEST--
$this in foreach
--FILE--
<?php
$a = [1];
foreach ($a as $this) {
    var_dump($this);
}
?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_001.php on line 3
