--TEST--
mb_check_encoding() with UTF-8: errors at every position around SIMD block boundaries
--EXTENSIONS--
mbstring
--FILE--
<?php
// Valid and invalid sequences, placed at every offset of strings with various
// lengths, so that they straddle the boundaries of 16, 32 and 64-byte blocks.
$sequences = [
    "\xC3\xA9", "\xE4\xB8\xAD", "\xF0\x9F\x98\x80", "\xF4\x8F\xBF\xBF", "\xED\x9F\xBF", "\xEE\x80\x80",
    "\x80", "\xBF", "\xC0\x80", "\xC1\xBF", "\xC2", "\xE0\x80\x80", "\xE0\xA0", "\xED\xA0\x80",
    "\xF0\x80\x80\x80", "\xF0\x9F\x98", "\xF4\x90\x80\x80", "\xF5\x80\x80\x80", "\xF8", "\xFF",
    "\xC3\xA9\xA9", "\xE4\xB8", "\xF0\x9F",
];
$fillers = ["a", "\xC3\xA9", "\xE4\xB8\xAD", "\xF0\x9F\x98\x80"];
$checked = 0;
$mismatches = 0;
foreach ($fillers as $filler) {
    for ($len = 0; $len <= 140; $len++) {
        $base = str_repeat($filler, intdiv($len, strlen($filler)) + 1);
        // Cut at a character boundary
        $base = mb_strcut($base, 0, $len, 'UTF-8');
        $n = strlen($base);
        for ($pos = 0; $pos <= $n; $pos++) {
            foreach ($sequences as $seq) {
                foreach ([false, true] as $replace) {
                    $s = substr($base, 0, $pos) . $seq . substr($base, $pos + ($replace ? strlen($seq) : 0));
                    $expected = preg_match('//u', $s) === 1;
                    $s = str_repeat($s, 1); // a fresh string, with no cached validity flag
                    if (mb_check_encoding($s, 'UTF-8') !== $expected) {
                        if ($mismatches++ < 10) {
                            echo "Mismatch: ", bin2hex($s), " expected ", var_export($expected, true), "\n";
                        }
                    }
                    $checked++;
                }
            }
        }
    }
}
echo $checked > 100000 ? "enough cases\n" : "too few cases: $checked\n";
echo "mismatches: $mismatches\n";
?>
--EXPECT--
enough cases
mismatches: 0
