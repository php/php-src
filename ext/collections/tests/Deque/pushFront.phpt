--TEST--
Collections\Deque unshift
--FILE--
<?php

$it = new Collections\Deque();
for ($i = 0; $i < 20; $i++) {
    $it->unshift("$i");
}
foreach ($it as $value) {
    echo "$value,";
}
echo "\n";
$values = [];
while (count($it) > 0) {
    $values[] = $it->shift();
}
echo json_encode($values), "\n";
?>
--EXPECT--
19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
["19","18","17","16","15","14","13","12","11","10","9","8","7","6","5","4","3","2","1","0"]