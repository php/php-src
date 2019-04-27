--TEST--
foreach() & undefined var
--FILE--
<?php

foreach($a as $val);

echo "Done\n";
?>
--EXPECTF--
Notice: Undefined variable: a in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
Done
