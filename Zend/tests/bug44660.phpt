--TEST--
Bug #44660 (Indexed and reference assignment to propery of non-object don't trigger warning)
--FILE--
<?php
$s = "hello";
$a = true;

echo "--> read access: ";
echo $a->p;

echo "\n--> direct assignment: ";
$a->p = $s;

echo "\n--> increment: ";
$a->p++;

echo "\n--> reference assignment:";
$a->p =& $s;

echo "\n--> reference assignment:";
$s =& $a->p;

echo "\n--> indexed assignment:";
$a->p[0] = $s;

echo "\n--> Confirm assignments have had no impact:\n";
var_dump($a);
?>
--EXPECTF--
--> read access: 
Notice: Trying to get property 'p' of non-object in %sbug44660.php on line 6

--> direct assignment: 
Warning: Attempt to assign property 'p' of non-object in %sbug44660.php on line 9

--> increment: 
Warning: Attempt to increment/decrement property 'p' of non-object in %sbug44660.php on line 12

--> reference assignment:
Warning: Attempt to modify property 'p' of non-object in %sbug44660.php on line 15

--> reference assignment:
Warning: Attempt to modify property 'p' of non-object in %sbug44660.php on line 18

--> indexed assignment:
Warning: Attempt to modify property 'p' of non-object in %sbug44660.php on line 21

--> Confirm assignments have had no impact:
bool(true)
