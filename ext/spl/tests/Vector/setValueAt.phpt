--TEST--
Vector offsetSet/set
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
expect_throws(fn() => $vec->offsetSet(0, strtoupper('value')));
expect_throws(fn() => $vec->set(0, strtoupper('value')));

echo "Test short vector\n";
$str = 'Test short vector';
$vec = new Vector(explode(' ', $str));
$vec->set(0, 'new');
$vec->offsetSet(2, strtoupper('test'));
echo json_encode($vec), "\n";
expect_throws(fn() => $vec->set(-1, strtoupper('value')));
expect_throws(fn() => $vec->set(3, 'end'));
expect_throws(fn() => $vec->set(PHP_INT_MAX, 'end'));

?>
--EXPECT--
Test empty vector
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Test short vector
["new","short","TEST"]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range