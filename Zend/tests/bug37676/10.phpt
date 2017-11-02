--TEST--
Bug #37676 (Coalesce non-warning)
--FILE--
<?php
$a = false;
var_dump($a[0][1] ?? 42); // Coalesce is fetched as an isset, which suppresses warnings.
?>
--EXPECTF--
int(42)
