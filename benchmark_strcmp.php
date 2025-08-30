<?php

function benchmark($name, $func) {
    // strcmp is extremely fast, so a very high iteration count is needed for meaningful results.
    $iterations = 2000000;
    $start = microtime(true);
    for ($i = 0; $i < $iterations; $i++) {
        $func();
    }
    $end = microtime(true);
    echo str_pad($name, 45) . ": " . number_format($end - $start, 6) . "s\n";
}

echo "Benchmarking current strcmp implementation...\n";

$long_string1 = str_repeat("abcdefghijklmnopqrstuvwxyz", 10); // 260 chars
$long_string2 = str_repeat("abcdefghijklmnopqrstuvwxyz", 10); // 260 chars, same content
$long_string_prefix = substr($long_string1, 0, -1); // 259 chars
$long_string_diff_start = "b" . substr($long_string1, 1);
$long_string_diff_end = substr($long_string1, 0, -1) . "A";


// ===== TEST CASES =====

// Case 1: Equal short strings
benchmark("Equal short strings", function() {
    strcmp("hello world", "hello world");
});

// Case 2: Equal long strings
benchmark("Equal long strings", function() use ($long_string1, $long_string2) {
    strcmp($long_string1, $long_string2);
});

// Case 3: Difference at beginning (should be very fast)
benchmark("Difference at beginning", function() use ($long_string1, $long_string_diff_start) {
    strcmp($long_string1, $long_string_diff_start);
});

// Case 4: Difference at end (should be slow)
benchmark("Difference at end", function() use ($long_string1, $long_string_diff_end) {
    strcmp($long_string1, $long_string_diff_end);
});

// Case 5: One string is a prefix of the other
benchmark("Prefix comparison", function() use ($long_string1, $long_string_prefix) {
    strcmp($long_string1, $long_string_prefix);
});

?>
