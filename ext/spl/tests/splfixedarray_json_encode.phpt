--TEST--
json_encode() on SplFixedArray
--FILE--
<?php

echo json_encode(new SplFixedArray()) . "\n";
echo json_encode(new SplFixedArray(1)) . "\n";

$a = new SplFixedArray(3);
$a[0] = 0;
$a[2] = 2;
echo json_encode($a) . "\n";

?>
--EXPECT--
[]
[null]
[0,null,2]
