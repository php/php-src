--TEST--
Bug #66179 (var_export() exports float as integer)
--FILE--
<?php

var_export(1.0);
echo PHP_EOL;
var_export(123.0);
echo PHP_EOL;
var_export(-1.0);
echo PHP_EOL;
var_export(-123.0);
echo PHP_EOL;
var_export(0.0);
echo PHP_EOL;
var_export(-0.0);
echo PHP_EOL;
var_export(10000000000000000.0);
echo PHP_EOL;

?>
--EXPECTF--
1.0
123.0
-1.0
-123.0
0.0
-0.0
10000000000000000.0
