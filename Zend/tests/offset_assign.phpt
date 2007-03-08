--TEST--
Crash on $x['x']['y'] += 1 when $x is string
--FILE--
<?php
$x = "a";
$x['x']['y'] += 1; 

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot use string offset as an array in %s on line %d
