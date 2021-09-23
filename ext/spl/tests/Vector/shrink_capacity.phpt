--TEST--
Vector pop() reduces count
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$s = 'test 123 test test test test test test';
$vec = new Vector(explode(' ', $s));
while (count($vec) > 0) {
    var_dump($vec->pop());
    printf("new count=%d\n", count($vec));
}
$vec->shrinkToFit();
printf("shrinkToFit\n");
printf("shrinkToFit\n");
$vec->push(new stdClass());
printf("it=%s count=%d\n", json_encode($vec), count($vec));
$vec->shrinkToFit();
printf("it=%s count=%d\n", json_encode($vec), count($vec));
?>
--EXPECT--
string(4) "test"
new count=7
string(4) "test"
new count=6
string(4) "test"
new count=5
string(4) "test"
new count=4
string(4) "test"
new count=3
string(4) "test"
new count=2
string(3) "123"
new count=1
string(4) "test"
new count=0
shrinkToFit
shrinkToFit
it=[{}] count=1
it=[{}] count=1