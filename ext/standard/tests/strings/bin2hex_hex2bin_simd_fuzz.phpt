--TEST--
bin2hex()/hex2bin() SIMD fast path: round-trip fuzz
--FILE--
<?php
mt_srand(1234567);

$fail = 0;
for ($i = 0; $i < 500; $i++) {
    $len = mt_rand(0, 200);
    $bytes = '';
    for ($j = 0; $j < $len; $j++) {
        $bytes .= chr(mt_rand(0, 255));
    }

    $hex = bin2hex($bytes);
    $roundtrip = bin2hex(hex2bin($hex));

    if ($roundtrip !== $hex) {
        echo "FAIL at i=$i len=$len\n";
        $fail++;
    }
}

echo $fail === 0 ? "done\n" : "$fail failures\n";
?>
--EXPECT--
done
