--TEST--
Bug #75700 incorrect number after json_encode
--FILE--
<?php
$a =  (1010*(0.00003*100))/100;

echo $a;
echo "\n";
echo json_encode($a);

--EXPECT--
0.0303
0.0303