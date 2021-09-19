--TEST--
Collections\Deque push/pop
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

echo "Test empty deque\n";
$it = new Collections\Deque([]);
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->pop());
$it->push(strtoupper('test'));
$it->push(['literal']);
$it->push(new stdClass());
$it[] = strtoupper('test2');
$it[] = false;
echo json_encode($it), "\n";
printf("count=%d\n", count($it));
var_dump($it->pop());
var_dump($it->pop());
var_dump($it->pop());
var_dump($it->pop());
echo "After popping 4 elements: ", json_encode($it->toArray()), "\n";
var_dump($it->pop());
echo json_encode($it), "\n";
printf("count=%d\n", count($it));

?>
--EXPECT--
Test empty deque
Caught UnderflowException: Cannot pop from empty Collections\Deque
Caught UnderflowException: Cannot pop from empty Collections\Deque
["TEST",["literal"],{},"TEST2",false]
count=5
bool(false)
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
count=0