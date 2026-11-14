--TEST--
Async\delay() argument validation and Future restrictions
--FILE--
<?php
foreach ([-1, NAN, INF] as $v) {
    try {
        Async\delay($v);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
try {
    new Async\Future;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone Async\delay(0);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    serialize(Async\delay(0));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Async\delay(): Argument #1 ($seconds) must be a finite number greater than or equal to 0
Async\delay(): Argument #1 ($seconds) must be a finite number greater than or equal to 0
Async\delay(): Argument #1 ($seconds) must be a finite number greater than or equal to 0
Call to private Async\Future::__construct() from global scope
Trying to clone an uncloneable object of class Async\Future
Serialization of 'Async\Future' is not allowed
