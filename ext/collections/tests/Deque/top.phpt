--TEST--
Collections\Deque last()/first()
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

$it = new Collections\Deque();
expect_throws(fn () => $it->first());
expect_throws(fn () => $it->last());
for ($i = 0; $i <= 3; $i++) {
    $it->push("last$i");
    $it->unshift("first$i");
}
echo $it->first(), "\n";
echo $it->last(), "\n";
echo "Removing elements\n";
echo $it->shift(), "\n";
echo $it->pop(), "\n";
echo "Inspecting elements after removal\n";
echo $it->first(), "\n";
echo $it->last(), "\n";
printf("count=%d values=%s\n", $it->count(), json_encode($it));

?>
--EXPECT--
Caught UnderflowException: Cannot read first value of empty Collections\Deque
Caught UnderflowException: Cannot read last value of empty Collections\Deque
first3
last3
Removing elements
first3
last3
Inspecting elements after removal
first2
last2
count=6 values=["first2","first1","first0","last0","last1","last2"]