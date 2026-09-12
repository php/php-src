--TEST--
json_encode() option flags on strings long enough to hit the SSE2 fast path
--FILE--
<?php
/* Same option-flag matrix as ext/json/tests/006.phpt and
 * json_encode_unescaped_slashes.phpt, but padded past 16 bytes on both
 * sides so the SSE2 fast path in php_json_escape_string() actually
 * engages before and after the special character (short strings never
 * exercise it). */

function check($label, $actual, $expected) {
    if ($actual === $expected) {
        echo "$label: OK\n";
    } else {
        echo "$label: FAIL\n";
        var_dump($expected, $actual);
    }
}

$pad = str_repeat('x', 20);

check('tag default', json_encode($pad . '<foo>' . $pad),
    '"' . $pad . '<foo>' . $pad . '"');
check('tag HEX_TAG', json_encode($pad . '<foo>' . $pad, JSON_HEX_TAG),
    '"' . $pad . "\\u003Cfoo\\u003E" . $pad . '"');

check('apos default', json_encode($pad . "'bar'" . $pad),
    '"' . $pad . "'bar'" . $pad . '"');
check('apos HEX_APOS', json_encode($pad . "'bar'" . $pad, JSON_HEX_APOS),
    '"' . $pad . "\\u0027bar\\u0027" . $pad . '"');

check('quot default', json_encode($pad . '"baz"' . $pad),
    '"' . $pad . '\"baz\"' . $pad . '"');
check('quot HEX_QUOT', json_encode($pad . '"baz"' . $pad, JSON_HEX_QUOT),
    '"' . $pad . "\\u0022baz\\u0022" . $pad . '"');

check('amp default', json_encode($pad . '&blong&' . $pad),
    '"' . $pad . '&blong&' . $pad . '"');
check('amp HEX_AMP', json_encode($pad . '&blong&' . $pad, JSON_HEX_AMP),
    '"' . $pad . "\\u0026blong\\u0026" . $pad . '"');

check('slash default', json_encode($pad . 'a/b' . $pad),
    '"' . $pad . 'a\/b' . $pad . '"');
check('slash UNESCAPED_SLASHES', json_encode($pad . 'a/b' . $pad, JSON_UNESCAPED_SLASHES),
    '"' . $pad . 'a/b' . $pad . '"');

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
tag default: OK
tag HEX_TAG: OK
apos default: OK
apos HEX_APOS: OK
quot default: OK
quot HEX_QUOT: OK
amp default: OK
amp HEX_AMP: OK
slash default: OK
slash UNESCAPED_SLASHES: OK
unicode default: OK
unicode UNESCAPED_UNICODE: OK
lineterm default: OK
lineterm UNESCAPED_UNICODE: OK
lineterm UNESCAPED_UNICODE|UNESCAPED_LINE_TERMINATORS: OK
