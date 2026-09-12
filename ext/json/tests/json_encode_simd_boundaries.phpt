--TEST--
json_encode() SIMD scanner boundaries
--FILE--
<?php

function check(string $input, int $flags, string|false $expected, string $label): void
{
    $actual = json_encode($input, $flags);
    if ($actual !== $expected) {
        printf(
            "FAIL %s\nExpected: %s\nActual:   %s\n",
            $label,
            var_export($expected, true),
            var_export($actual, true),
        );
    }
}

foreach ([31, 32, 33, 47, 48, 63, 64, 65] as $length) {
    $input = str_repeat('a', $length);
    check($input, 0, '"' . $input . '"', "safe input of length $length");
}

$escapes = [
    "\x00" => '\u0000',
    "\x08" => '\b',
    "\x09" => '\t',
    "\x0a" => '\n',
    "\x0c" => '\f',
    "\x0d" => '\r',
    "\x1f" => '\u001f',
    '"' => '\"',
    '&' => '&',
    "'" => "'",
    '/' => '\/',
    '<' => '<',
    '>' => '>',
    '\\' => '\\\\',
    "\u{100}" => '\u0100',
];

foreach ([15, 16, 31, 32, 47, 48] as $position) {
    $prefix = str_repeat('a', $position);
    $suffix = str_repeat('b', 64 - $position);
    foreach ($escapes as $character => $escape) {
        check(
            $prefix . $character . $suffix,
            0,
            '"' . $prefix . $escape . $suffix . '"',
            sprintf("character %s at position %d", bin2hex($character), $position),
        );
    }
}

check(
    str_repeat('a', 32) . '<>&\'"',
    JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_HEX_QUOT,
    '"' . str_repeat('a', 32) . '\u003C\u003E\u0026\u0027\u0022"',
    'JSON_HEX_* flags',
);
check(
    str_repeat('a', 32) . '/',
    JSON_UNESCAPED_SLASHES,
    '"' . str_repeat('a', 32) . '/"',
    'JSON_UNESCAPED_SLASHES',
);
check(
    str_repeat('a', 32) . "\xff",
    JSON_INVALID_UTF8_SUBSTITUTE,
    '"' . str_repeat('a', 32) . '\ufffd"',
    'JSON_INVALID_UTF8_SUBSTITUTE',
);
check(
    str_repeat('a', 32) . "\xff",
    JSON_INVALID_UTF8_IGNORE,
    '"' . str_repeat('a', 32) . '"',
    'JSON_INVALID_UTF8_IGNORE',
);
check(
    str_repeat('a', 32) . "\xff",
    0,
    false,
    'invalid UTF-8',
);

echo "Done\n";
?>
--EXPECT--
Done
