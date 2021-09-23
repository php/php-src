--TEST--
Vector push(...$args)/pop
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
$vec = new Vector([]);
printf("count=%d\n", $vec->count());
expect_throws(fn() => $vec->pop());
expect_throws(fn() => $vec->pop());
$vec->push(strtoupper('test'));
$vec->push(['literal']);
$vec->push(new stdClass());
$vec[] = strtoupper('test2');
$vec[] = null;
echo json_encode($vec), "\n";
printf("count=%d\n", count($vec));
var_dump($vec->pop());
var_dump($vec->pop());
var_dump($vec->pop());
var_dump($vec->pop());
echo "After popping 4 elements: ", json_encode($vec->toArray()), "\n";
var_dump($vec->pop());
echo json_encode($vec), "\n";
printf("count=%d\n", count($vec));
echo "After shrinkToFit\n";
$vec->shrinkToFit();
echo json_encode($vec), "\n";
printf("count=%d\n", count($vec));
echo "After pushing variadic args\n";
$vec->push(strtoupper('test'), 'other', 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
echo json_encode($vec), "\n";
printf("count=%d\n", count($vec));
$vec->push('a', 'b');
echo json_encode($vec), "\n";
// After pushing no args (like array_push, e.g. for variadic argument lists)
$vec->push();
echo json_encode($vec), "\n";

?>
--EXPECT--
Test empty vector
count=0
Caught UnderflowException: Cannot pop from empty Vector
Caught UnderflowException: Cannot pop from empty Vector
["TEST",["literal"],{},"TEST2",null]
count=5
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
count=0
After shrinkToFit
[]
count=0
After pushing variadic args
["TEST","other",1,2,3,4,5,6,7,8,9,10]
count=12
["TEST","other",1,2,3,4,5,6,7,8,9,10,"a","b"]
["TEST","other",1,2,3,4,5,6,7,8,9,10,"a","b"]