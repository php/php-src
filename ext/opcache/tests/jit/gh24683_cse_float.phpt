--TEST--
GH-24683 (Floating-point CSE survives a CV reassignment)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip requires 64-bit integers'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
opcache.file_update_protection=0
--FILE--
<?php
function pressure(array $input, array &$out, int $tag) {
    $x = $input[0];
    $a = $x + 11.25;
    $out[0] = $a;
    if ($tag === 91) {
        return -1;
    }
    $a = $input[1];
    $out[1] = $a;
    $a = $x + 11.25;
    $out[2] = $a;
    return $a;
}

$wrong = 0;
for ($k = 0; $k < 800; $k++) {
    $input = [$k % 13, 2];
    $out = [];
    $tag = ($k > 700 && $k % 17 === 0) ? 91 : 0;
    $result = pressure($input, $out, $tag);
    $value = $input[0] + 11.25;
    $expected = $tag === 91 ? [-1, [$value]] : [$value, [$value, 2, $value]];
    if ([$result, $out] !== $expected) {
        $wrong++;
    }
}
echo "wrong: $wrong\n";
--EXPECT--
wrong: 0
