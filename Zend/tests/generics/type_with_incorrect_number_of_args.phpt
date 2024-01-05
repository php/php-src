--TEST--
Type that uses an incorrect number of generic args
--FILE--
<?php

class C1<T> {}
class C2<T1, T2> {}
class C3<T1, T2, T3 = int> {}

function test1(C1<int, int> $param) {}
function test2(C2<int, int, int> $param) {}
function test3(C2<int> $param) {}
function test4(C3<int, int, int, int> $param) {}
function test5(C3<int> $param) {}

try {
    test1(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test3(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test4(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test5(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class C1 expects exactly 1 generic argument, but 2 provided
Class C2 expects exactly 2 generic arguments, but 3 provided
Class C2 expects exactly 2 generic arguments, but 1 provided
Class C3 expects at most 3 generic arguments, but 4 provided
Class C3 expects at least 2 generic arguments, but 1 provided
