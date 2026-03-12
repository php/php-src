--TEST--
GH-16649: array_splice with replacement array when destructor modifies array
--FILE--
<?php
class C {
    function __destruct() {
        global $arr;
        $arr["modified"] = "by_destructor";
    }
}

$arr = ["a", new C, "b"];
$replacement = ["replacement1", "replacement2"];

try {
    array_splice($arr, 1, 1, $replacement);
    echo "ERROR: Should have thrown exception\n";
} catch (Error $e) {
    echo "Exception caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception caught: Array was modified during array_splice operation
