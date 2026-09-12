--TEST--
json_encode() SSE2 fast-path boundary handling
--FILE--
<?php
/* Regression tests for the SSE2 chunked fast path in
 * php_json_escape_string() (ext/json/json_encoder.c). The fast path scans
 * 16-byte chunks and resumes scanning right after each escaped byte, so
 * exercise lengths and escape/codepoint positions around that boundary. */

function check($label, $actual, $expected) {
    if ($actual === $expected) {
        echo "$label: OK\n";
    } else {
        echo "$label: FAIL\n";
        var_dump($expected, $actual);
    }
}

// 1. Pure clean ASCII at/around the 16-byte chunk boundary.
foreach ([15, 16, 17, 31, 32, 33] as $len) {
    $s = str_repeat('a', $len);
    check("clean len=$len", json_encode($s), '"' . $s . '"');
}

// 2. A single escapable byte at each offset around the boundary.
foreach ([0, 15, 16, 17] as $off) {
    $len = 24;
    $s = str_repeat('a', $off) . '"' . str_repeat('a', $len - $off - 1);
    $expected = '"' . str_repeat('a', $off) . '\\"' . str_repeat('a', $len - $off - 1) . '"';
    check("quote at offset=$off", json_encode($s), $expected);
}

// 3. A 3-byte UTF-8 sequence (EUR SIGN, U+20AC) straddling the boundary.
// Checked both with the default \uXXXX escaping and with
// JSON_UNESCAPED_UNICODE, which appends the raw UTF-8 bytes instead.
foreach ([13, 14, 15, 16] as $off) {
    $len = 20;
    $s = str_repeat('a', $off) . "\xe2\x82\xac" . str_repeat('a', $len - $off - 3);
    $tail = str_repeat('a', $len - $off - 3);
    check("3-byte utf8 at offset=$off", json_encode($s),
        '"' . str_repeat('a', $off) . "\\u20ac" . $tail . '"');
    check("3-byte utf8 at offset=$off, UNESCAPED_UNICODE", json_encode($s, JSON_UNESCAPED_UNICODE),
        '"' . str_repeat('a', $off) . '€' . $tail . '"');
}

// 4. A 4-byte UTF-8 sequence (surrogate pair, U+1F600) straddling the boundary.
foreach ([12, 13, 14, 15, 16] as $off) {
    $len = 20;
    $s = str_repeat('a', $off) . "\xf0\x9f\x98\x80" . str_repeat('a', $len - $off - 4);
    $tail = str_repeat('a', $len - $off - 4);
    check("4-byte utf8 at offset=$off", json_encode($s),
        '"' . str_repeat('a', $off) . "\\ud83d\\ude00" . $tail . '"');
    check("4-byte utf8 at offset=$off, UNESCAPED_UNICODE", json_encode($s, JSON_UNESCAPED_UNICODE),
        '"' . str_repeat('a', $off) . '😀' . $tail . '"');
}

// 5. Invalid UTF-8 straddling the boundary: confirm the checkpoint/rollback
// and each INVALID_UTF8_* option still land correctly after the fast path
// has already appended bytes.
foreach ([14, 15, 16, 17] as $off) {
    $len = 20;
    $s = str_repeat('a', $off) . "\xb0" . str_repeat('a', $len - $off - 1);
    $tail = str_repeat('a', $len - $off - 1);

    check("invalid utf8 at offset=$off, no flag", json_encode($s), false);

    check("invalid utf8 at offset=$off, IGNORE",
        json_encode($s, JSON_INVALID_UTF8_IGNORE),
        '"' . str_repeat('a', $off) . $tail . '"');

    check("invalid utf8 at offset=$off, SUBSTITUTE",
        json_encode($s, JSON_INVALID_UTF8_SUBSTITUTE),
        '"' . str_repeat('a', $off) . "\\ufffd" . $tail . '"');
}
?>
--EXPECT--
clean len=15: OK
clean len=16: OK
clean len=17: OK
clean len=31: OK
clean len=32: OK
clean len=33: OK
quote at offset=0: OK
quote at offset=15: OK
quote at offset=16: OK
quote at offset=17: OK
3-byte utf8 at offset=13: OK
3-byte utf8 at offset=13, UNESCAPED_UNICODE: OK
3-byte utf8 at offset=14: OK
3-byte utf8 at offset=14, UNESCAPED_UNICODE: OK
3-byte utf8 at offset=15: OK
3-byte utf8 at offset=15, UNESCAPED_UNICODE: OK
3-byte utf8 at offset=16: OK
3-byte utf8 at offset=16, UNESCAPED_UNICODE: OK
4-byte utf8 at offset=12: OK
4-byte utf8 at offset=12, UNESCAPED_UNICODE: OK
4-byte utf8 at offset=13: OK
4-byte utf8 at offset=13, UNESCAPED_UNICODE: OK
4-byte utf8 at offset=14: OK
4-byte utf8 at offset=14, UNESCAPED_UNICODE: OK
4-byte utf8 at offset=15: OK
4-byte utf8 at offset=15, UNESCAPED_UNICODE: OK
4-byte utf8 at offset=16: OK
4-byte utf8 at offset=16, UNESCAPED_UNICODE: OK
invalid utf8 at offset=14, no flag: OK
invalid utf8 at offset=14, IGNORE: OK
invalid utf8 at offset=14, SUBSTITUTE: OK
invalid utf8 at offset=15, no flag: OK
invalid utf8 at offset=15, IGNORE: OK
invalid utf8 at offset=15, SUBSTITUTE: OK
invalid utf8 at offset=16, no flag: OK
invalid utf8 at offset=16, IGNORE: OK
invalid utf8 at offset=16, SUBSTITUTE: OK
invalid utf8 at offset=17, no flag: OK
invalid utf8 at offset=17, IGNORE: OK
invalid utf8 at offset=17, SUBSTITUTE: OK
