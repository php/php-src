--TEST--
Bug #47168 (printf of floating point variable prints maximum of 40 decimal places)
--FILE--
<?php

$dyadic = 0.00000000000045474735088646411895751953125;
var_dump(printf ("%1.41f\n",unserialize(serialize($dyadic))));

?>
--EXPECT--
0.00000000000045474735088646411895751953125
int(44)
