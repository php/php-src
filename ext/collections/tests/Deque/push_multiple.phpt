--TEST--
Collections\Deque constructed from array
--FILE--
<?php
// discards keys
$it = new Collections\Deque();
// no-op to support any length of variadic arguments
$it->push();
printf("it=%s count=%d\n", json_encode($it), $it->count());
$it->push(...range(0, 19));
printf("it=%s count=%d\n", json_encode($it), $it->count());
$it->pushFront(...range(0, 19));
printf("it=%s count=%d\n", json_encode($it), $it->count());
?>
--EXPECT--
it=[] count=0
it=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19] count=20
it=[19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19] count=40
