--TEST--
json_encode() option flags on strings long enough to hit the SIMD fast path
--FILE--
<?php
/* Same option-flag matrix as ext/json/tests/006.phpt and
 * json_encode_unescaped_slashes.phpt, but padded past 16 bytes on both
 * sides so the SIMD (SSE2/NEON) fast path in php_json_escape_string()
 * actually engages before and after the special character (short strings
 * never exercise it). The offsets 0/15/16/17 additionally place the
 * option-gated character right at the 16-byte chunk boundary. */

function check($label, $actual, $expected) {
    if ($actual === $expected) {
        echo "$label: OK\n";
    } else {
        echo "$label: FAIL\n";
        var_dump($expected, $actual);
    }
}

$pad = str_repeat('x', 20);

foreach ([0, 15, 16, 17] as $off) {
    $lead = str_repeat('x', $off);

    check("tag default off=$off", json_encode($lead . '<foo>' . $pad),
        '"' . $lead . '<foo>' . $pad . '"');
    check("tag HEX_TAG off=$off", json_encode($lead . '<foo>' . $pad, JSON_HEX_TAG),
        '"' . $lead . "\\u003Cfoo\\u003E" . $pad . '"');

    check("apos default off=$off", json_encode($lead . "'bar'" . $pad),
        '"' . $lead . "'bar'" . $pad . '"');
    check("apos HEX_APOS off=$off", json_encode($lead . "'bar'" . $pad, JSON_HEX_APOS),
        '"' . $lead . "\\u0027bar\\u0027" . $pad . '"');

    check("quot default off=$off", json_encode($lead . '"baz"' . $pad),
        '"' . $lead . '\"baz\"' . $pad . '"');
    check("quot HEX_QUOT off=$off", json_encode($lead . '"baz"' . $pad, JSON_HEX_QUOT),
        '"' . $lead . "\\u0022baz\\u0022" . $pad . '"');

    check("amp default off=$off", json_encode($lead . '&blong&' . $pad),
        '"' . $lead . '&blong&' . $pad . '"');
    check("amp HEX_AMP off=$off", json_encode($lead . '&blong&' . $pad, JSON_HEX_AMP),
        '"' . $lead . "\\u0026blong\\u0026" . $pad . '"');

    check("slash default off=$off", json_encode($lead . 'a/b' . $pad),
        '"' . $lead . 'a\/b' . $pad . '"');
    check("slash UNESCAPED_SLASHES off=$off", json_encode($lead . 'a/b' . $pad, JSON_UNESCAPED_SLASHES),
        '"' . $lead . 'a/b' . $pad . '"');
}

check('unicode default', json_encode($pad . "\xc3\xa9" . $pad),
    '"' . $pad . "\\u00e9" . $pad . '"');
check('unicode UNESCAPED_UNICODE', json_encode($pad . "\xc3\xa9" . $pad, JSON_UNESCAPED_UNICODE),
    '"' . $pad . 'é' . $pad . '"');

check('lineterm default', json_encode($pad . "\xe2\x80\xa8" . $pad),
    '"' . $pad . "\\u2028" . $pad . '"');
check('lineterm UNESCAPED_UNICODE', json_encode($pad . "\xe2\x80\xa8" . $pad, JSON_UNESCAPED_UNICODE),
    '"' . $pad . "\\u2028" . $pad . '"');
check('lineterm UNESCAPED_UNICODE|UNESCAPED_LINE_TERMINATORS',
    json_encode($pad . "\xe2\x80\xa8" . $pad, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_LINE_TERMINATORS),
    '"' . $pad . "\xe2\x80\xa8" . $pad . '"');
?>
--EXPECT--
tag default off=0: OK
tag HEX_TAG off=0: OK
apos default off=0: OK
apos HEX_APOS off=0: OK
quot default off=0: OK
quot HEX_QUOT off=0: OK
amp default off=0: OK
amp HEX_AMP off=0: OK
slash default off=0: OK
slash UNESCAPED_SLASHES off=0: OK
tag default off=15: OK
tag HEX_TAG off=15: OK
apos default off=15: OK
apos HEX_APOS off=15: OK
quot default off=15: OK
quot HEX_QUOT off=15: OK
amp default off=15: OK
amp HEX_AMP off=15: OK
slash default off=15: OK
slash UNESCAPED_SLASHES off=15: OK
tag default off=16: OK
tag HEX_TAG off=16: OK
apos default off=16: OK
apos HEX_APOS off=16: OK
quot default off=16: OK
quot HEX_QUOT off=16: OK
amp default off=16: OK
amp HEX_AMP off=16: OK
slash default off=16: OK
slash UNESCAPED_SLASHES off=16: OK
tag default off=17: OK
tag HEX_TAG off=17: OK
apos default off=17: OK
apos HEX_APOS off=17: OK
quot default off=17: OK
quot HEX_QUOT off=17: OK
amp default off=17: OK
amp HEX_AMP off=17: OK
slash default off=17: OK
slash UNESCAPED_SLASHES off=17: OK
unicode default: OK
unicode UNESCAPED_UNICODE: OK
lineterm default: OK
lineterm UNESCAPED_UNICODE: OK
lineterm UNESCAPED_UNICODE|UNESCAPED_LINE_TERMINATORS: OK
