--TEST--
OSS-Fuzz #427814452
--FILE--
<?php

try {
    false |> assert(...);
} catch (\Throwable $e) {
    echo $e::class, ': "', $e->getMessage(), '"', "\n";
}
try {
    0 |> "assert"(...);
} catch (\Throwable $e) {
    echo $e::class, ': "', $e->getMessage(), '"', "\n";
}
try {
    false |> ("a"."ssert")(...);
} catch (\Throwable $e) {
    echo $e::class, ': "', $e->getMessage(), '"', "\n";
}

?>
--EXPECT--
AssertionError: ""
AssertionError: ""
AssertionError: ""
