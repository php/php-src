--TEST--
Collections\Deque insert and remove at offset
--FILE--
<?php
$prefix = 'v';
$a = new Collections\Deque();
$a->insert(0, "{$prefix}2");
$a->insert(0, "{$prefix}0", "{$prefix}1");
echo json_encode($a), "\n";
$a->insert(3, 'third', "{$prefix}4");
echo json_encode($a), "\n";
unset($a[2]);
while (count($a) > 0) {
    unset($a[0]);
    echo json_encode($a), "\n";
}
?>
--EXPECT--
["v0","v1","v2"]
["v0","v1","v2","third","v4"]
["v1","third","v4"]
["third","v4"]
["v4"]
[]
