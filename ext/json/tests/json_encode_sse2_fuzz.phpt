--TEST--
json_encode()/json_decode() round-trip across many lengths (SSE2 fast path)
--FILE--
<?php
/* The SSE2 fast path in php_json_escape_string() scans fixed 16-byte
 * chunks, so a string containing a mix of clean ASCII, escapable ASCII,
 * and multi-byte UTF-8 sequences will place its "interesting" bytes at
 * every possible offset relative to a chunk boundary as its length
 * varies. Round-tripping through json_decode() is a strong, deterministic
 * oracle here since the decoder is untouched by this patch. */

$alphabet = [
    'a', 'b', 'c', ' ', '"', '\\', '/', '<', '>', '&', '\'',
    "\t", "\n", "\r", "\x01",
    "\xc3\xa9",         // 2-byte UTF-8 (e)
    "\xe2\x82\xac",     // 3-byte UTF-8 (EUR SIGN)
    "\xf0\x9f\x98\x80", // 4-byte UTF-8 (surrogate pair on encode)
];
$na = count($alphabet);

$optionSets = [
    0,
    JSON_UNESCAPED_SLASHES,
    JSON_UNESCAPED_UNICODE,
    JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_HEX_QUOT,
];

$failures = [];
for ($count = 0; $count <= 80; $count++) {
    $s = '';
    for ($i = 0; $i < $count; $i++) {
        // Deterministic index (no RNG) so the corpus is reproducible.
        $idx = ($count * 31 + $i * 17) % $na;
        $s .= $alphabet[$idx];
    }

    foreach ($optionSets as $opts) {
        $encoded = json_encode($s, $opts);
        if ($encoded === false) {
            $failures[] = "count=$count opts=$opts: encode failed";
            continue;
        }
        $decoded = json_decode($encoded);
        if ($decoded !== $s) {
            $failures[] = "count=$count opts=$opts: round-trip mismatch";
        }
    }
}

if ($failures) {
    echo implode("\n", $failures), "\n";
} else {
    echo "OK\n";
}
?>
--EXPECT--
OK
