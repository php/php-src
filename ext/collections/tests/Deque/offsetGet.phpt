--TEST--
Collections\Deque offsetGet/get
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
expect_throws(fn() => (new ReflectionClass(Collections\Deque::class))->newInstanceWithoutConstructor());
$it = new Collections\Deque(['first' => new stdClass()]);
var_dump($it->offsetGet(0));
expect_throws(fn() => $it->offsetSet(1,'x'));
expect_throws(fn() => $it->offsetUnset(PHP_INT_MIN));
expect_throws(fn() => $it->offsetUnset(-1));
expect_throws(fn() => $it->offsetUnset(count($it)));
var_dump($it->offsetGet('0'));
echo "offsetExists checks\n";
var_dump($it->offsetExists(1));
var_dump($it->offsetExists('1'));
var_dump($it->offsetExists(PHP_INT_MAX));
var_dump($it->offsetExists(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet(-1));
echo "Invalid offsetGet calls\n";
expect_throws(fn() => $it->offsetGet(PHP_INT_MAX));
expect_throws(fn() => $it->offsetGet(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet('1'));
expect_throws(fn() => $it->offsetGet('invalid'));
expect_throws(fn() => $it[['invalid']]);
expect_throws(fn() => $it->offsetUnset(PHP_INT_MAX));
expect_throws(fn() => $it->offsetSet(PHP_INT_MAX,'x'));
expect_throws(function () use ($it) { unset($it[-1]); });
var_dump($it->getIterator());
?>
--EXPECT--
Caught ReflectionException: Class Collections\Deque is an internal class marked as final that cannot be instantiated without invoking its constructor
object(stdClass)#1 (0) {
}
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
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
Caught TypeError: Illegal offset type
Caught TypeError: Illegal offset type
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
object(InternalIterator)#4 (0) {
}