--TEST--
complex cases of variable assignment - 003
--FILE--
<?php

$var = 0.213123123;
$var1 = &$var;
$var = $var[1];

var_dump($var);
var_dump($var1);

echo "Done\n";
?>
--EXPECTF--	
Notice: Trying to get index of a non-array in %s on line 5
NULL
NULL
Done
