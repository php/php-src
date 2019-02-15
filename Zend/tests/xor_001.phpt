--TEST--
XORing arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array();

$c = $a ^ $b;
var_dump($c);

echo "Done\n";
?>
--EXPECT--
int(1)
Done
