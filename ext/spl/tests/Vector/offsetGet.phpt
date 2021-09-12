--TEST--
Vector offsetGet/valueAt
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
expect_throws(fn() => (new ReflectionClass(Vector::class))->newInstanceWithoutConstructor());
$it = new Vector([new stdClass()]);
var_dump($it->offsetGet(0));
var_dump($it->valueAt(0));
expect_throws(fn() => $it->offsetSet(1,'x'));
expect_throws(fn() => $it->offsetUnset(0));
var_dump($it->offsetGet('0'));
echo "offsetExists checks\n";
var_dump($it->offsetExists(1));
var_dump($it->offsetExists('1'));
var_dump($it->offsetExists(PHP_INT_MAX));
var_dump($it->offsetExists(PHP_INT_MIN));
expect_throws(fn() => $it->valueAt(1));
expect_throws(fn() => $it->valueAt(-1));
echo "Invalid offsetGet calls\n";
expect_throws(fn() => $it->offsetGet(PHP_INT_MAX));
expect_throws(fn() => $it->offsetGet(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->valueAt(PHP_INT_MAX));
expect_throws(fn() => $it->valueAt(PHP_INT_MIN));
expect_throws(fn() => $it->valueAt(1));
expect_throws(fn() => $it->valueAt(-1));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet('1'));
expect_throws(fn() => $it->offsetGet('invalid'));
expect_throws(fn() => $it->valueAt('invalid'));
expect_throws(fn() => $it[['invalid']]);
expect_throws(fn() => $it->offsetUnset(PHP_INT_MAX));
expect_throws(fn() => $it->offsetSet(PHP_INT_MAX,'x'));
expect_throws(function () use ($it) { unset($it[0]); });
var_dump($it->getIterator());
?>
--EXPECT--
Caught ReflectionException: Class Vector is an internal class marked as final that cannot be instantiated without invoking its constructor
object(stdClass)#1 (0) {
}
object(stdClass)#1 (0) {
}
Caught OutOfBoundsException: Index out of range
Caught RuntimeException: Vector does not support offsetUnset - elements must be set to null or removed by resizing
object(stdClass)#1 (0) {
}
offsetExists checks
bool(false)
bool(false)
bool(false)
bool(false)
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Invalid offsetGet calls
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught TypeError: Illegal offset type
Caught TypeError: Vector::valueAt(): Argument #1 ($offset) must be of type int, string given
Caught TypeError: Illegal offset type
Caught RuntimeException: Vector does not support offsetUnset - elements must be set to null or removed by resizing
Caught OutOfBoundsException: Index out of range
Caught RuntimeException: Vector does not support offsetUnset - elements must be set to null or removed by resizing
object(InternalIterator)#2 (0) {
}