--TEST--
GH-19926 (Assertion failure zend_hash_internal_pointer_reset_ex)
--FILE--
<?php
class ThrowingDestructor {
    function __destruct() {
        throw new Exception();
    }
}

$arr = [new ThrowingDestructor(), new ThrowingDestructor()];

try {
    array_splice($arr, 0, 2);
} catch (Exception $e) {
    echo "Exception caught, no assertion failure\n";
}
?>
--EXPECT--
Exception caught, no assertion failure
