--TEST--
GH-16649: array_splice UAF with destructor modifying array (original case)
--FILE--
<?php
function resize_arr() {
    global $arr;
    for ($i = 0; $i < 10; $i++) {
        $arr[$i] = $i;
    }
}

class C {
    function __destruct() {
        resize_arr();
        return "3";
    }
}

$arr = ["a" => "1", "3" => new C, "2" => "2"];

try {
    array_splice($arr, 1, 2);
    echo "ERROR: Should have thrown exception\n";
} catch (Error $e) {
    echo "Exception caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception caught: Array was modified during array_splice operation
