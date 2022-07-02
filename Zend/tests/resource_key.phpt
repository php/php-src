--TEST--
Behavior of resources as array keys
--FILE--
<?php

$r = fopen(__FILE__, 'r');
$a = [];
echo "Assign:";
$a[$r] = 1;
echo "Add assign:";
$a[$r] += 1;
echo "Inc:";
$a[$r]++;
echo "Get:";
var_dump($a[$r]);
echo "Isset:";
var_dump(isset($a[$r]));
echo "Unset:";
unset($a[$r]);

?>
--EXPECTF--
Assign:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
Add assign:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
Inc:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
Get:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
int(3)
Isset:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
bool(true)
Unset:
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
