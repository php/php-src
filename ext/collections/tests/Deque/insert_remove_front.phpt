--TEST--
Collections\Deque insert and remove at front offset is efficient
--FILE--
<?php
// Performance of this remains amortized constant time.
// It will move elements from the start of the deque when the inserted/removed offset is closer to the start.
$deque = new Collections\Deque();
for ($i = 0; $i < 10000; $i++) {
    $deque->insert(0, $i % 10);
}
$total = 0;
while (count($deque) > 0) {
    $total += $deque[0];
    unset($deque[0]);
}
printf("total: %d\n", $total);
?>
--EXPECT--
total: 45000
