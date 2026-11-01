--TEST--
Str\Matcher counts beyond INT32_MAX and rejects PHP_INT_MAX overflow on 32-bit
--EXTENSIONS--
strmatch
--FILE--
<?php
$keywords = [];
for ($i = 1; $i <= 256; $i++) {
    $keywords[] = str_repeat('a', $i);
}
$matcher = new Str\Matcher($keywords);
try {
    $count = $matcher->countMatches(str_repeat('a', 8388736));
    echo PHP_INT_SIZE === 8 && $count === 2147483776 ? "correct boundary behavior\n" : "FAIL\n";
} catch (Error $e) {
    echo PHP_INT_SIZE === 4 && $e->getMessage() === 'Match count exceeds PHP_INT_MAX'
        ? "correct boundary behavior\n" : $e::class . ': ' . $e->getMessage() . "\n";
}
?>
--EXPECT--
correct boundary behavior
