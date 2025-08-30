<?php

function benchmark($name, $func) {
    $start = microtime(true);
    $func();
    $end = microtime(true);
    printf("%-40s: %8.4f seconds\n", $name, $end - $start);
}

const LARGE_INT = 10000000;
const LARGE_FLOAT = 10000000.0;

// --- Integer Benchmarks ---
benchmark("Integer Range (1 to 10,000,000)", function() {
    $a = range(1, LARGE_INT);
});

benchmark("Integer Range (10,000,000 to 1)", function() {
    $a = range(LARGE_INT, 1);
});

benchmark("Integer Range with Step (1 to 10,000,000, step 2)", function() {
    $a = range(1, LARGE_INT, 2);
});


// --- Float Benchmarks ---
benchmark("Float Range (1.0 to 10,000,000.0)", function() {
    $a = range(1.0, LARGE_FLOAT);
});

benchmark("Float Range with Step (10,000,000.0 to 1.0, step 2.5)", function() {
    $a = range(LARGE_FLOAT, 1.0, 2.5);
});


// --- Character Benchmark ---
benchmark("Character Range ('a' to 'z')", function() {
    $a = range('a', 'z');
});

echo "\n";

?>
