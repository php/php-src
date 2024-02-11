--TEST--
errmsg: can't use [] for reading
--FILE--
<?php

$a = array();
$b = $a[];

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
