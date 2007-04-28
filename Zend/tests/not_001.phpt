--TEST--
bitwise NOT, doubles and strings
--FILE--
<?php

$d = 23.67;
$s = "48484.22";
$s1 = "test";
$s2 = "some";

$s = ~$d;
var_dump($s);

$s1 = ~$s2;
var_dump(bin2hex($s1));

echo "Done\n";
?>
--EXPECTF--	
int(-24)
string(8) "8c90929a"
Done
