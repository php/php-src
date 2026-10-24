--TEST--
GH-24214 (Shared assignment values must be restored at function JIT loop entries)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
opcache.optimization_level=0x7FFEBFDF
--FILE--
<?php
function markTokens(array $tokens): array {
    $n = count($tokens);
    for ($i = 0; $i < $n; ++$i) {
        $tokens[$i][4] = true;
    }
    return $tokens;
}

var_dump(markTokens([[], [], []]) === [[4 => true], [4 => true], [4 => true]]);

foreach ([3, 0, 1, 64] as $size) {
    $tokens = markTokens(array_fill(0, $size, []));
    var_dump(count($tokens) === $size);
    var_dump($tokens === array_fill(0, $size, [4 => true]));
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
