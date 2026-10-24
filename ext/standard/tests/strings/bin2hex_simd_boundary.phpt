--TEST--
bin2hex() SIMD fast path: boundary lengths
--FILE--
<?php
// Hardcoded golden vector: 20 bytes is long enough to engage the SIMD path
// (needs >= 16 bytes) and still leaves a scalar remainder tail (4 bytes),
// encoding the known byte sequence 0..19.
$bin = '';
for ($i = 0; $i < 20; $i++) {
    $bin .= chr($i);
}
echo bin2hex($bin), "\n";

$all = '';
for ($i = 0; $i < 256; $i++) {
    $all .= chr($i);
}
$source = str_repeat($all, 5); // 1280 bytes, enough for len 1000 + offsets

$lengths = [0, 1, 15, 16, 17, 20, 31, 32, 33, 1000];

foreach ($lengths as $len) {
    $expected = '';
    for ($i = 0; $i < $len; $i++) {
        $expected .= sprintf('%02x', ord($source[$i]));
    }
    $actual = bin2hex(substr($source, 0, $len));
    if ($actual !== $expected) {
        echo "FAIL len=$len\n";
    }
}

// A long string spanning many SIMD chunk iterations.
$big = str_repeat($all, 300); // 76800 bytes
$expectedBig = '';
for ($i = 0; $i < strlen($big); $i++) {
    $expectedBig .= sprintf('%02x', ord($big[$i]));
}
if (bin2hex($big) !== $expectedBig) {
    echo "FAIL long string\n";
}

echo "done\n";
?>
--EXPECT--
000102030405060708090a0b0c0d0e0f10111213
done
