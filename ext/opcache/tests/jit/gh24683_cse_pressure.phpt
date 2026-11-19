--TEST--
GH-24683 (Bound copies do not exhaust the tracing JIT spill frame)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip requires 64-bit integers'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
; Isolate function-entry compilation from a caller loop that could fall back.
opcache.jit_hot_loop=0
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
opcache.file_update_protection=0
opcache.jit_debug=0x14000
--FILE--
<?php
function safe_pressure(array $in, array &$out, int $tag) {
    $x0 = $in[0];
    $x1 = $in[1];
    $x2 = $in[2];
    $x3 = $in[3];
    $x4 = $in[4];
    $a0 = $x0 + 11.25; $out[0] = $a0;
    $a1 = $x1 + 12.25; $out[1] = $a1;
    $a2 = $x2 + 13.25; $out[2] = $a2;
    $a3 = $x3 + 14.25; $out[3] = $a3;
    $a4 = $x4 + 15.25; $out[4] = $a4;
    if ($tag === 91) {
        return -1;
    }
    $b0 = $x0 + 11.25; $out[10] = $b0;
    $b1 = $x1 + 12.25; $out[11] = $b1;
    $b2 = $x2 + 13.25; $out[12] = $b2;
    $b3 = $x3 + 14.25; $out[13] = $b3;
    $b4 = $x4 + 15.25; $out[14] = $b4;
    return $a0 + $a1 + $a2 + $a3 + $a4;
}

$hash = hash_init('sha256');
for ($k = 0; $k < 800; $k++) {
    $in = range(1, 10);
    $in[0] = $k % 13;
    $out = [];
    $result = safe_pressure($in, $out, ($k > 700 && $k % 17 === 0) ? 91 : 0);
    hash_update($hash, serialize([$result, $out]));
}
echo hash_final($hash), "\n";
--EXPECTREGEX--
(?:---- TRACE [0-9]+ compiled\n\n)+01fee1fb9cedccbd27d2b9c70613b77b7095e42bdbcc80eb820337d461d21ec8
