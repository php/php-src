--TEST--
base64_decode() and base64_encode(): whitespace, padding and invalid bytes around vector block boundaries
--FILE--
<?php
/* Reference decoder, character by character. */
function ref_decode(string $s, bool $strict): string|false {
    static $alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
    $bits = 0; $nbits = 0; $i = 0; $padding = 0; $out = '';
    for ($k = 0; $k < strlen($s); $k++) {
        $ch = $s[$k];
        if ($ch === '=') { $padding++; continue; }
        $v = strpos($alphabet, $ch);
        if ($v === false) {
            if (!$strict || strpos(" \t\r\n", $ch) !== false) continue;
            return false;
        }
        if ($strict && $padding) return false;
        $bits = ($bits << 6) | $v; $nbits += 6; $i++;
        if ($nbits >= 8) { $nbits -= 8; $out .= chr(($bits >> $nbits) & 0xff); }
    }
    if ($strict && $i % 4 == 1) return false;
    if ($strict && $padding && ($padding > 2 || ($i + $padding) % 4 != 0)) return false;
    return $out;
}

function check(string $s): void {
    foreach ([false, true] as $strict) {
        if (base64_decode($s, $strict) !== ref_decode($s, $strict)) {
            echo "Mismatch (strict=", var_export($strict, true), "): ", json_encode($s), "\n";
        }
    }
}

$data = '';
for ($i = 0; $i < 150; $i++) {
    $data .= chr(($i * 151 + 7) & 0xff);
}

for ($n = 0; $n <= 150; $n++) {
    $raw = substr($data, 0, $n);
    $enc = base64_encode($raw);
    if (base64_decode($enc, true) !== $raw || ref_decode($enc, true) !== $raw) {
        echo "Round trip failed for length $n\n";
    }
    if (base64_encode($raw) !== rtrim(base64_encode($raw), '=') . str_repeat('=', (3 - $n % 3) % 3)) {
        echo "Padding wrong for length $n\n";
    }
    check($enc);
    check(rtrim($enc, '='));
    check(chunk_split($enc, 76, "\r\n"));
    check(chunk_split($enc, 64, "\n"));
}

/* One inserted byte at every position of inputs that span one or two blocks. */
foreach ([60, 63, 64, 65, 68, 100, 127, 128, 129, 132] as $len) {
    $enc = substr(str_repeat(base64_encode($data), 2), 0, $len);
    foreach (["\n", " ", "=", "==", "*", "\0", "\x80", "\f"] as $ins) {
        for ($pos = 0; $pos <= $len; $pos++) {
            check(substr($enc, 0, $pos) . $ins . substr($enc, $pos));
            check(substr($enc, 0, $pos) . $ins . substr($enc, $pos) . "==");
        }
    }
}

/* Long runs of whitespace, and padding followed by whitespace. */
$enc = base64_encode($data);
for ($pos = 0; $pos <= strlen($enc); $pos += 7) {
    check(substr($enc, 0, $pos) . str_repeat(" ", 130) . substr($enc, $pos));
}
check($enc . str_repeat("\n", 200));
check(rtrim($enc, '=') . "=\n=" . str_repeat(" ", 100));
check($enc . str_repeat(" ", 100) . "A");

/* Long inputs, decoded in several batches; then an invalid byte or padding late in the input. */
$enc = base64_encode(str_repeat($data, 30));
foreach ([76, 64, 3, 1] as $w) {
    check(chunk_split($enc, $w, "\r\n"));
}
$s = chunk_split($enc, 76, "\n");
foreach ([1000, 3000, strlen($s) - 70] as $pos) {
    check(substr($s, 0, $pos) . "*" . substr($s, $pos));
    check(substr($s, 0, $pos) . "=" . substr($s, $pos));
}
echo "Done\n";
?>
--EXPECT--
Done
