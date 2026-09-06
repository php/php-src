--TEST--
GH-16649: array_splice UAF when array is converted from packed to hash
--FILE--
<?php
class C {
    function __destruct() {
        global $arr;
        // array is converted from packed to hash
        $arr["str"] = 0;
    }
}

$arr = ["1", new C, "2"];

try {
    array_splice($arr, 1, 2);
    echo "ERROR: Should have thrown exception\n";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Array was modified during array_splice operation
