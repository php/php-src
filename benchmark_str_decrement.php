<?php

function benchmark($name, $func) {
    $start = microtime(true);
    // Using a smaller iteration count because string operations can be slow
    for ($i = 0; $i < 20000; $i++) {
        $func();
    }
    $end = microtime(true);
    echo "$name: " . ($end - $start) . "s\n";
}

echo "Benchmarking current str_decrement implementation...\n";

// Case 1: Simple numeric string, no borrow needed.
$s1 = "123456789123456789";
benchmark("Simple numeric string", function() use ($s1) {
    str_decrement($s1);
});

// Case 2: Long numeric string with full borrow. This is the key case for my optimization.
$s2 = "1" . str_repeat("0", 50);
benchmark("Long numeric string with full borrow", function() use ($s2) {
    str_decrement($s2);
});

// Case 3: Simple alphanumeric string, no borrow needed.
$s3 = "abcdefg9";
benchmark("Simple alphanumeric string", function() use ($s3) {
    str_decrement($s3);
});

// Case 4: Long alphanumeric string with numeric borrow.
$s4 = "b" . str_repeat("0", 50);
benchmark("Long alphanumeric with numeric borrow", function() use ($s4) {
    str_decrement($s4);
});

// Case 5: Long alphanumeric string with letter borrow.
$s5 = "z" . str_repeat("a", 50);
benchmark("Long alphanumeric with letter borrow", function() use ($s5) {
    str_decrement($s5);
});

// Case 6: A long string that does not trigger the leading-zero optimization.
$s6 = "2" . str_repeat("0", 50);
benchmark("Long numeric string without leading-zero result", function() use ($s6) {
    str_decrement($s6);
});

?>
