--TEST--
Vector push/pop
--FILE--
<?php

function expect_throws(Closure $cb): void {
    try {
        $cb();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

echo "Test empty vector\n";
$it = new Vector([]);
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->pop());
$it->push(strtoupper('test'));
$it->push(['literal']);
$it->push(new stdClass());
$it[] = strtoupper('test2');
$it[] = null;
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
var_dump($it->pop());
var_dump($it->pop());
var_dump($it->pop());
var_dump($it->pop());
echo "After popping 4 elements: ", json_encode($it->toArray()), "\n";
var_dump($it->pop());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
echo "After shrinkToFit\n";
$it->shrinkToFit();
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());

?>
--EXPECT--
Test empty vector
count=0 capacity=0
Caught UnderflowException: Cannot pop from empty Vector
Caught UnderflowException: Cannot pop from empty Vector
["TEST",["literal"],{},"TEST2",null]
count=5 capacity=8
NULL
string(5) "TEST2"
object(stdClass)#2 (0) {
}
array(1) {
  [0]=>
  string(7) "literal"
}
After popping 4 elements: ["TEST"]
string(4) "TEST"
[]
count=0 capacity=4
After shrinkToFit
[]
count=0 capacity=0