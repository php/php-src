--TEST--
complex cases of variable assignment - 004
--FILE--
<?php

$var = "intergalactic";
$var1 = "space";
$var2 = &$var1;

$var = $var2;

var_dump($var);
var_dump($var1);
var_dump($var2);

echo "Done\n";
?>
--EXPECT--
unicode(5) "space"
unicode(5) "space"
unicode(5) "space"
Done
