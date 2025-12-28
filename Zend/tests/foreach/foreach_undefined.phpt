--TEST--
foreach() & undefined var
--FILE--
<?php

foreach($a as $val);

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d

Warning: foreach() argument must be of type array|object, null given in %s on line %d
Done
