--TEST--
Partial application variation uaf in cleanup unfinished calls
--FILE--
<?php
function test(){}

try {
    test(...)(?);
} catch (Error $ex) {
    echo "OK";
}
?>
--EXPECT--
OK
