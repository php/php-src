--TEST--
Bug #52001 (Memory allocation problems after using variable variables)
--FILE--
<?php
a(0,$$var);

$temp1=1;
$temp2=2;
var_dump($temp1);

function a($b,$c) {}
?>
--EXPECTF--
Notice: Undefined variable: var in %sbug52001.php on line 2

Notice: Undefined variable:  in %sbug52001.php on line 2
int(1)
