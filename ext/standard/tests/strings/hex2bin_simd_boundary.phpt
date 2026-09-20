--TEST--
hex2bin() SIMD fast path: boundary lengths and invalid-byte-at-every-offset
--FILE--
<?php
// Hardcoded golden vector: 40 hex chars (20 bytes) is long enough to engage
// the SIMD path (needs >= 32 hex chars) and still leaves a scalar remainder
// tail (4 bytes), decoding to the known byte sequence 0..19.
$hex = "000102030405060708090a0b0c0d0e0f10111213";
$bin = hex2bin($hex);
$bytes = [];
for ($i = 0; $i < strlen($bin); $i++) {
    $bytes[] = ord($bin[$i]);
}
echo implode(' ', $bytes), "\n";

$hex = str_repeat('0123456789abcdefABCDEF', 10);
$hex = substr($hex, 0, 2000);
if (strlen($hex) % 2 !== 0) {
    $hex = substr($hex, 0, -1);
}

$lengths = [0, 2, 30, 32, 34, 40, 62, 64, 66, 2000];
foreach ($lengths as $len) {
    $chunk = substr($hex, 0, $len);
    $bin = hex2bin($chunk);
    $roundtrip = bin2hex($bin);
    if (strtolower($chunk) !== $roundtrip) {
        echo "FAIL len=$len\n";
    }
}

// Invalid byte at every offset 0..63 relative to the 32-byte SIMD chunk
// boundary (two full SIMD chunks), across a few different bad-byte classes.
$valid64 = str_repeat('0123456789abcdef', 4);
$badChars = ['g', 'G', 'z', '!', ' ', "\xFF", "\x00", '-'];
foreach (range(0, 63) as $pos) {
    foreach ($badChars as $bad) {
        $s = $valid64;
        $s[$pos] = $bad;
        $result = @hex2bin($s);
        if ($result !== false) {
            echo "FAIL: pos=$pos char=" . bin2hex($bad) . " was not rejected\n";
        }
    }
}

// A long hex string spanning many SIMD chunk iterations.
$bigHex = str_repeat('0123456789abcdefABCDEF', 3000);
if (strlen($bigHex) % 2 !== 0) {
    $bigHex = substr($bigHex, 0, -1);
}
$bigBin = hex2bin($bigHex);
if (strtolower($bigHex) !== bin2hex($bigBin)) {
    echo "FAIL long hex string\n";
}

echo "done\n";
?>
--EXPECT--
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
done
