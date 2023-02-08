--TEST--
Collections\Deque push repeatedly
--FILE--
<?php

$dq = new Collections\Deque();
for ($i = 0; $i < 8; $i++) {
    $dq->push("v$i");
}
$dq->popFront();
$dq->push("extra");
printf("dq=%s count=%d\n", json_encode($dq), $dq->count());
for ($i = 0; $i < 7; $i++) {
    $value = $dq->popFront();
    printf("popped %s\n", $value);
}
// Collections\Deque should reclaim memory once roughly a quarter of the memory is actually used.
printf("dq=%s count=%d\n", json_encode($dq), $dq->count());
var_dump($dq);
$dq->clear();
printf("dq=%s count=%d\n", json_encode($dq), $dq->count());
var_dump($dq);
?>
--EXPECT--
dq=["v1","v2","v3","v4","v5","v6","v7","extra"] count=8
popped v1
popped v2
popped v3
popped v4
popped v5
popped v6
popped v7
dq=["extra"] count=1
object(Collections\Deque)#1 (1) {
  [0]=>
  string(5) "extra"
}
dq=[] count=0
object(Collections\Deque)#1 (0) {
}
