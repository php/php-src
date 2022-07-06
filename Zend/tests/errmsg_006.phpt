--TEST--
errmsg: can't use [] for reading
--FILE--
<?php

$a = array();
$b = $a[];

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
