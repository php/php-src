--TEST--
GH-17190 (Assertion failure ext/opcache/jit/ir/ir_gcm.c)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
opcache.jit=1254
opcache.jit_hot_func=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
$a = [2147483647,2147483647,2147483647,3,0,0,32,2147483584,127];
echo @crc32(json_encode(bitwise_small_split($a))) . "\n";

function bitwise_small_split($val) {
    $split = 8;
    $len = count($val);
    while ($i != $len) {
        if (!$overflow) {
            $remaining -= $split;
            $overflow = $split <= $remaining ? 0 : $split - $remaining;
        } elseif (++$i != $len) {
            $fusion[$i] >>= $overflow;
            $remaining = 31 - $overflow;
            $overflow = $split <= $remaining ? 0 : $split - $remaining;
        }
    }
}
?>
--EXPECT--
634125391
