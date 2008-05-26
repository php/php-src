--TEST--
complex cases of variable assignment - 002
--FILE--
<?php

$var = "intergalactic";
$var1 = &$var;
$var = $var[5];

var_dump($var);
var_dump($var1);

echo "Done\n";
?>
--EXPECT--
unicode(1) "g"
unicode(1) "g"
Done
