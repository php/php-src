--TEST--
json_encode() escapes every byte value 0x00-0xFF correctly
--FILE--
<?php

/* Independent reference implementation of PHP's JSON string-escaping
 * rules, deliberately not sharing any logic with ext/json's C
 * implementation, so this test verifies behavior rather than trivially
 * agreeing with whatever the implementation currently does. */
function expected_escape(int $b, int $options): string {
    switch ($b) {
        case 0x22: return ($options & JSON_HEX_QUOT) ? "\\u0022" : '\"';
        case 0x5c: return '\\\\';
        case 0x2f: return ($options & JSON_UNESCAPED_SLASHES) ? '/' : '\/';
        case 0x08: return '\b';
        case 0x0c: return '\f';
        case 0x0a: return '\n';
        case 0x0d: return '\r';
        case 0x09: return '\t';
        case 0x3c: return ($options & JSON_HEX_TAG) ? "\\u003C" : '<';
        case 0x3e: return ($options & JSON_HEX_TAG) ? "\\u003E" : '>';
        case 0x26: return ($options & JSON_HEX_AMP) ? "\\u0026" : '&';
        case 0x27: return ($options & JSON_HEX_APOS) ? "\\u0027" : "'";
    }
    if ($b < 0x20) {
        return sprintf('\u%04x', $b);
    }
    return chr($b);
}

function check_ascii_range(int $options): void {
    for ($b = 0x00; $b < 0x80; $b++) {
        $expected = '"' . expected_escape($b, $options) . '"';
        $actual = json_encode(chr($b), $options);
        if ($actual !== $expected) {
            printf("MISMATCH (options=%d) at byte 0x%02x: expected %s got %s\n",
                $options, $b, var_export($expected, true), var_export($actual, true));
        }
    }
}

function check_lone_high_bytes(): void {
    /* A single byte >= 0x80 is never valid UTF-8 on its own -- every one
     * of these must be rejected as invalid UTF-8, not silently passed
     * through unescaped. */
    for ($b = 0x80; $b <= 0xff; $b++) {
        $actual = json_encode(chr($b));
        if ($actual !== false) {
            printf("MISMATCH at byte 0x%02x: expected false (invalid UTF-8) got %s\n",
                $b, var_export($actual, true));
        }
        if (json_last_error() !== JSON_ERROR_UTF8) {
            printf("MISMATCH at byte 0x%02x: expected JSON_ERROR_UTF8, got error code %d\n",
                $b, json_last_error());
        }
    }
}

check_ascii_range(0);
check_ascii_range(JSON_UNESCAPED_SLASHES);
check_ascii_range(JSON_HEX_QUOT | JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS);
check_ascii_range(JSON_HEX_QUOT | JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_UNESCAPED_SLASHES);
check_lone_high_bytes();

echo "Done\n";
?>
--EXPECT--
Done
