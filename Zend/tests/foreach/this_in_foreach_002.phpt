--TEST--
$this in foreach
--FILE--
<?php
$a = [1];
foreach ($a as $this => $dummy) {
    var_dump($this);
}
?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_002.php on line 3
