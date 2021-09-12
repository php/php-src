--TEST--
Vector offsetSet/setValueAt
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
expect_throws(fn() => $it->offsetSet(0, strtoupper('value')));
expect_throws(fn() => $it->setValueAt(0, strtoupper('value')));

echo "Test short vector\n";
$str = 'Test short vector';
$it = new Vector(explode(' ', $str));
$it->setValueAt(0, 'new');
$it->offsetSet(2, strtoupper('test'));
echo json_encode($it), "\n";
expect_throws(fn() => $it->setValueAt(-1, strtoupper('value')));
expect_throws(fn() => $it->setValueAt(3, 'end'));
expect_throws(fn() => $it->setValueAt(PHP_INT_MAX, 'end'));

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