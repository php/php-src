<?php

function benchmark($name, $func) {
    $start = microtime(true);
    // A lower iteration count because str_ireplace can be very slow with arrays on long strings.
    for ($i = 0; $i < 500; $i++) {
        $func();
    }
    $end = microtime(true);
    echo str_pad($name, 50) . ": " . number_format($end - $start, 6) . "s\n";
}

echo "Benchmarking current str_ireplace implementation...\n";

// A long string (~22KB) with a variety of characters to search for.
$long_haystack = str_repeat("The quick brown fox jumps over the lazy dog. ", 500);

// An array of search terms that will all be found.
$search_array = ['the', 'quick', 'brown', 'fox', 'jumps', 'over', 'lazy', 'dog'];
$replace_array = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'];

// An array of search terms that will not be found.
$search_array_no_match = ['xylophone', 'yak', 'zebra', 'walrus', 'vulture', 'unicorn', 'tiger', 'snake'];

// ===== TEST CASES =====

// Case 1: Simple string search (no arrays) for baseline comparison.
benchmark("Simple string search", function() use ($long_haystack) {
    str_ireplace('fox', 'cat', $long_haystack);
});

// Case 2: Array search, single replace. THIS IS THE PRIMARY OPTIMIZATION TARGET.
benchmark("Array search, single replacement", function() use ($long_haystack, $search_array) {
    str_ireplace($search_array, 'REPLACED', $long_haystack);
});

// Case 3: Array search, array replace. THIS IS THE PRIMARY OPTIMIZATION TARGET.
benchmark("Array search, array replacement", function() use ($long_haystack, $search_array, $replace_array) {
    str_ireplace($search_array, $replace_array, $long_haystack);
});

// Case 4: Array search with no matches. To ensure no performance regression.
benchmark("Array search, no matches", function() use ($long_haystack, $search_array_no_match) {
    str_ireplace($search_array_no_match, 'REPLACED', $long_haystack);
});

?>
