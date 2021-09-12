--TEST--
Vector pop() shrinks capacity
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$s = 'test 123 test test test test test test';
$it = new Vector(explode(' ', $s));
while (count($it) > 0) {
    var_dump($it->pop());
    printf("capacity=%d\n", $it->capacity());
}
$it->shrinkToFit();
printf("shrinkToFit capacity=%d\n", $it->capacity());
printf("shrinkToFit capacity=%d\n", $it->capacity());
$it->push(new stdClass());
printf("it=%s count=%d capacity=%d\n", json_encode($it), count($it), $it->capacity());
$it->shrinkToFit();
printf("it=%s count=%d capacity=%d\n", json_encode($it), count($it), $it->capacity());
?>
--EXPECT--
string(4) "test"
capacity=8
string(4) "test"
capacity=8
string(4) "test"
capacity=8
string(4) "test"
capacity=8
string(4) "test"
capacity=8
string(4) "test"
capacity=8
string(3) "123"
capacity=8
string(4) "test"
capacity=4
shrinkToFit capacity=0
shrinkToFit capacity=0
it=[{}] count=1 capacity=4
it=[{}] count=1 capacity=1
