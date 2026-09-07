--TEST--
Do not constant fold increment of array
--FILE--
<?php
function test_inc_array() {
    $a = [];
    $a++;
}
function test_inc_partial_array($k) {
    $a = [];
    $a[$k] = 0;
    $a++;
}
try {
    test_inc_array();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test_inc_partial_array(0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Cannot increment array
TypeError: Cannot increment array
