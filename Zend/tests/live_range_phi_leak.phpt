--TEST--
Missing live range if part of phi
--FILE--
<?php
function doThrow() {
    throw new Exception("Test");
}
function test($k) {
    // The 0 gives the QM_ASSIGN a non-refcounted type.
    $res[$k ? $k : 0] = doThrow();
}
try {
    test(new stdClass);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Test
