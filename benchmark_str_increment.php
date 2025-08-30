<?php

function benchmark($name, $func) {
    $start = microtime(true);
    for ($i = 0; $i < 20000; $i++) {
        $func();
    }
    $end = microtime(true);
    echo "$name: " . ($end - $start) . "s\n";
}

echo "Benchmarking current str_increment implementation...\n";

// Case 1: Simple numeric string, no carry needed.
$s1 = "123456788";
benchmark("Simple numeric string", function() use ($s1) {
    str_increment($s1);
});

// Case 2: Long numeric string with full carry. This is the key case for my optimization.
$s2 = str_repeat("9", 50);
benchmark("Long numeric string with full carry", function() use ($s2) {
    str_increment($s2);
});

// Case 3: Simple alphanumeric string, no carry needed.
$s3 = "abcde8";
benchmark("Simple alphanumeric string", function() use ($s3) {
    str_increment($s3);
});

// Case 4: Long alphanumeric string with numeric carry.
$s4 = "a" . str_repeat("9", 50);
benchmark("Long alphanumeric with numeric carry", function() use ($s4) {
    str_increment($s4);
});

// Case 5: Long alphanumeric string with full letter and numeric carry.
$s5 = "z" . str_repeat("9", 50);
benchmark("Long alphanumeric with full carry", function() use ($s5) {
    str_increment($s5);
});

// Case 6: A long string that does not trigger reallocation.
$s6 = "8" . str_repeat("9", 50);
benchmark("Long numeric string without reallocation", function() use ($s6) {
    str_increment($s6);
});

?>
