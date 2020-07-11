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
Warning: Trying to access array offset on value of type float in %s on line %d
NULL
NULL
Done
