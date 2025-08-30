<?php

function benchmark($name, $func) {
    $start = microtime(true);
    // This function is very fast, so a high iteration count is needed.
    for ($i = 0; $i < 500000; $i++) {
        $func();
    }
    $end = microtime(true);
    echo str_pad($name, 40) . ": " . number_format($end - $start, 6) . "s\n";
}

echo "Benchmarking current str_ends_with implementation...\n";

$long_haystack = "This is a very long string that is used for benchmarking purposes to see how the function performs with a significant amount of data to process.";
$long_needle_match = "a significant amount of data to process.";
$long_needle_no_match = "a significant amount of data to process!";

// Case 1: Haystack shorter than needle (fast path)
benchmark("Haystack shorter than needle", function() {
    str_ends_with("short", "this is much longer");
});

// Case 2: Matching short needle
benchmark("Matching short needle", function() use ($long_haystack) {
    str_ends_with($long_haystack, "process.");
});

// Case 3: Non-matching short needle
benchmark("Non-matching short needle", function() use ($long_haystack) {
    str_ends_with($long_haystack, "process!");
});

// Case 4: Matching long needle
benchmark("Matching long needle", function() use ($long_haystack, $long_needle_match) {
    str_ends_with($long_haystack, $long_needle_match);
});

// Case 5: Non-matching long needle
benchmark("Non-matching long needle", function() use ($long_haystack, $long_needle_no_match) {
    str_ends_with($long_haystack, $long_needle_no_match);
});

// Case 6: Matching empty needle (edge case)
benchmark("Matching empty needle", function() use ($long_haystack) {
    str_ends_with($long_haystack, "");
});

?>
