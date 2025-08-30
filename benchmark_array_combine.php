<?php

function benchmark($name, $func) {
    $start = microtime(true);
    // Run the function multiple times to get a more stable reading
    for ($i = 0; $i < 10; $i++) {
        $func();
    }
    $end = microtime(true);
    printf("%-40s: %8.4f seconds\n", $name, ($end - $start));
}

const LARGE_SIZE = 1000000;

// --- Scenario 1: Packed Indexed Arrays (Target for optimization) ---
$indexed_keys = range(0, LARGE_SIZE - 1);
$indexed_values = range(0, LARGE_SIZE - 1);

benchmark("Packed Indexed Arrays", function() use ($indexed_keys, $indexed_values) {
    $a = array_combine($indexed_keys, $indexed_values);
});


// --- Scenario 2: Associative Arrays (Check for regressions) ---
$assoc_keys = [];
$assoc_values = [];
for ($i = 0; $i < LARGE_SIZE; $i++) {
    $assoc_keys[] = "key_" . $i;
    $assoc_values[] = $i;
}

benchmark("Associative Arrays", function() use ($assoc_keys, $assoc_values) {
    $a = array_combine($assoc_keys, $assoc_values);
});

echo "\n";

?>
