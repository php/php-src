--TEST--
Bug #11917 (primitives seem to be passed via reference instead of by value under some conditions when JIT is enabled on windows)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
opcache.jit_buffer_size=64M
opcache.jit=1255
opcache.jit_max_root_traces=10000000
opcache.jit_max_side_traces=10000000
opcache.jit_max_exit_counters=1000000
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
$a = [2147483647,2147483647,2147483647,3,0,0,32,2147483584,127];
echo crc32(json_encode(bitwise_small_split($a))) . "\n";
echo crc32(json_encode(bitwise_small_split($a))) . "\n";
echo crc32(json_encode(bitwise_small_split($a))) . "\n";
echo crc32(json_encode(bitwise_small_split($a))) . "\n";

function bitwise_small_split($val)
{
    $split = 8;
    $vals = [];

    $mask = (1 << $split) - 1;

    $i = $overflow = 0;
    $len = count($val);
    $val[] = 0;
    $remaining = 31;

    while ($i != $len) {
        $digit = $val[$i] & $mask;

        $val[$i] >>= $split;
        if (!$overflow) {
            $remaining -= $split;
            $overflow = $split <= $remaining ? 0 : $split - $remaining;

            if (!$remaining) {
                $i++;
                $remaining = 31;
                $overflow = 0;
            }
        } elseif (++$i != $len) {
            $tempmask = (1 << $overflow) - 1;
            $digit |= ($val[$i] & $tempmask) << $remaining;
            $val[$i] >>= $overflow;
            $remaining = 31 - $overflow;
            $overflow = $split <= $remaining ? 0 : $split - $remaining;
        }

        $vals[] = $digit;
    }

    while ($vals[count($vals) - 1] == 0) {
        unset($vals[count($vals) - 1]);
    }

    return array_reverse($vals);
}
?>
--EXPECT--
48207660
48207660
48207660
48207660
