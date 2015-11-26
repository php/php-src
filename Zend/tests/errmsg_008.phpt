--TEST--
errmsg: can't use [] for unsetting
--FILE--
<?php

$a = array();
unset($a[]);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot use [] for unsetting in %s on line %d
