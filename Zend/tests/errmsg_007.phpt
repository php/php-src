--TEST--
errmsg: can't use [] for reading - 2
--FILE--
<?php

$a = array();
isset($a[]);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d
