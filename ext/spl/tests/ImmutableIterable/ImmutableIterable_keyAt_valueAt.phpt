--TEST--
ImmutableIterable keyAt and valueAt()
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
expect_throws(fn() => (new ReflectionClass(ImmutableIterable::class))->newInstanceWithoutConstructor());
$emptyIt = new ImmutableIterable([]);
expect_throws(fn() => $emptyIt->keyAt(0));
expect_throws(fn() => $emptyIt->keyAt(PHP_INT_MAX));
expect_throws(fn() => $emptyIt->keyAt(-1));
$it = new ImmutableIterable(['first' => new stdClass()]);
var_dump($it->keyAt(0));
var_dump($it->valueAt(0));
expect_throws(fn() => $it->keyAt(PHP_INT_MAX));
expect_throws(fn() => $it->keyAt(PHP_INT_MIN));
expect_throws(fn() => $it->keyAt(1));
expect_throws(fn() => $it->keyAt(-1));
expect_throws(fn() => $it->valueAt(1));
expect_throws(fn() => $it->valueAt(-1));
?>
--EXPECT--
Caught ReflectionException: Class ImmutableIterable is an internal class marked as final that cannot be instantiated without invoking its constructor
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
string(5) "first"
object(stdClass)#1 (0) {
}
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
