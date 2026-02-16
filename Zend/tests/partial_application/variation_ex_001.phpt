--TEST--
PFA variation: UAF in cleanup unfinished calls
--FILE--
<?php
function test($a){}

try {
    test(1, ...)(?);
} catch (Error $ex) {
    echo $ex::class, ": ", $ex->getMessage(), "\n";
}
?>
--EXPECTF--
ArgumentCountError: Partial application of {closure:%s:%d}() expects at most 0 arguments, 1 given
