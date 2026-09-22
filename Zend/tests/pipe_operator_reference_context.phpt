--TEST--
Fix GH-19476: Pipe operator with function returning by reference
--FILE--
<?php

function &get_ref($_): string {
    static $a = "original";

    $a .= " ".$_;

    return $a;
}

function &test_pipe_ref(): string {
    return "input" |> get_ref(...);
}

$ref = &test_pipe_ref();
echo "Before: " . $ref . "\n";
$ref = "changed";
echo "After: " . test_pipe_ref() . "\n";

?>
--EXPECT--
Before: original input
After: changed input
