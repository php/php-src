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
Notice: Cannot get offset of a non-array variable in %s on line %d
NULL
NULL
Done
