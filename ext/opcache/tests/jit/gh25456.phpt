--TEST--
GH-25456 (Tracing JIT: wrong values after an inner loop)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
--FILE--
<?php
declare(strict_types=1);

function f(int $bits, int $avail, array $codes, int $n): array
{
    $out = [];
    for ($i = 0; $i < $n; ++$i) {
        while ($avail <= 48) {
            $avail += 8;
        }
        $code = $codes[0];
        $val = ($bits >> $avail) & ((1 << $code) - 1);
        if ($code > 16) {
            while ($avail <= 48) {
            }
        }
        if ($i < $n - 1) {
            while ($avail <= 48) {
            }
        }
        $out[] = $val;
    }
    return $out;
}

echo implode(',', array_unique(f(0x123456789ABCDEF, 30, [4], 300))), "\n";
?>
--EXPECT--
4
