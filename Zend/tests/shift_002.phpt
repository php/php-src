--TEST--
shifting strings right
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";

$s >>= 2;
var_dump($s);

$s1 >>= 1;
var_dump($s1);

$s2 >>= 3;
var_dump($s2);

echo "Done\n";
?>
--EXPECTF--	
int(30)

Warning: A non-numeric value encountered in %s on line %d
int(0)

Notice: A non well formed numeric value encountered in %s on line %d
int(5668)
Done
