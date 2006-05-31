--TEST--
using resource as array offset
--INI--
error_reporting=8191
--FILE--
<?php

$fp = fopen(__FILE__, 'r');

$array = array(1,2,3,4,5,6,7);

var_dump($array[$fp]);

echo "Done\n";
?>
--EXPECTF--	
Strict Standards: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
int(%d)
Done
