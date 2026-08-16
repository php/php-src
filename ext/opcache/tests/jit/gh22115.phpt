--TEST--
GH-22115: Tracing JIT drops loop-PHI CV register from SNAPSHOT after intermediate op1_def
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
opcache.jit=tracing
--FILE--
<?php
function f(array $meta, int $n): int {
    $s = 0;
    $i = 0;
    $sink = 0;
    while ($i < $n) {
        $copy = $s;
        $sink += $copy;
        $m = (int) $meta[$s];
        if (($m & 0xFF) === 1) {
            $i++;
            $s = ($m >> 16) & 0xFFFF;
            continue;
        }
        return $s;
    }
    return $s;
}
$meta = [0 => 1 | (1 << 16), 1 => 2 | (2 << 16)];
for ($w = 0; $w < 2000; $w++) f($meta, 3);
var_dump(f($meta, 3));
?>
--EXPECT--
int(1)
