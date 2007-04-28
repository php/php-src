--TEST--
shifting strings left
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";

$s <<= 2;
var_dump($s);

$s1 <<= 1;
var_dump($s1);

$s2 <<= 3;
var_dump($s2);

echo "Done\n";
?>
--EXPECTF--	
int(492)
int(0)
int(362760)
Done
