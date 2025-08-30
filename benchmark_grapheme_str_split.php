<?php

function benchmark($name, $func) {
    // A relatively low iteration count because grapheme operations can be intensive.
    $iterations = 500;
    $start = microtime(true);
    for ($i = 0; $i < $iterations; $i++) {
        $func();
    }
    $end = microtime(true);
    echo str_pad($name, 45) . ": " . number_format($end - $start, 6) . "s\n";
}

echo "Benchmarking current grapheme_str_split implementation...\n";

// A long string (~50KB) mixing ASCII, multi-byte, and complex graphemes.
$long_string = str_repeat("Hello world! Это тест. The quick brown 👨‍👩‍👧‍👦 fox. Á, B́, Ć.", 200);

// ===== TEST CASES =====

// Case 1: Simple ASCII string
benchmark("Simple ASCII string", function() {
    grapheme_str_split("abcdefghijklmnopqrstuvwxyz");
});

// Case 2: Multi-byte UTF-8 string (Cyrillic)
benchmark("Multi-byte UTF-8 string (Cyrillic)", function() {
    grapheme_str_split("абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
});

// Case 3: Complex Graphemes (Combining Marks)
// 'e' with 3 combining marks is one grapheme
benchmark("Complex Graphemes (Combining Marks)", function() {
    grapheme_str_split("é̄̃");
});

// Case 4: Complex Graphemes (Emoji)
// Family emoji and woman with skin tone modifier are single graphemes
benchmark("Complex Graphemes (Emoji)", function() {
    grapheme_str_split("👨‍👩‍👧‍👦👩🏽‍💻");
});

// Case 5: Long mixed string
benchmark("Long mixed string", function() use ($long_string) {
    grapheme_str_split($long_string);
});

?>
