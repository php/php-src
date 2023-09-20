--TEST--
Bug #11917 (primitives seem to be passed via reference instead of by value under some conditions when JIT is enabled on windows)
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
