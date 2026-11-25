--TEST--
grapheme_strlen() and grapheme_extract() with a non-ASCII byte or CR LF at every position
--EXTENSIONS--
intl
--FILE--
<?php
$ascii = 'abcdefghijklmnopq';
foreach (["\u{00E9}", "\u{1F600}", "\xFF", "\x80", "\r\n", "\r", "\n", "\rx\n", "\rx\r\n"] as $insert) {
    $lengths = [];
    for ($pos = 0; $pos <= strlen($ascii); $pos++) {
        $lengths[] = var_export(grapheme_strlen(substr($ascii, 0, $pos) . $insert . substr($ascii, $pos)), true);
    }
    echo bin2hex($insert), ': ', implode(' ', $lengths), "\n";
}

foreach (["abc\r\n", "abcdefgh\r\nij", "abcdefg\r\nij"] as $s) {
    $parts = [];
    for ($size = 1; $size <= strlen($s); $size++) {
        $next = null;
        $parts[] = json_encode(grapheme_extract($s, $size, GRAPHEME_EXTR_COUNT, 0, $next)) . " $next";
    }
    echo implode('; ', $parts), "\n";
}
?>
--EXPECT--
c3a9: 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18
f09f9880: 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18
ff: NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL
80: NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL
0d0a: 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18
0d: 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18
0a: 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18 18
0d780a: 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
0d780d0a: 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
"a" 1; "ab" 2; "abc" 3; "abc\r\n" 5; "abc\r\n" 5
"a" 1; "ab" 2; "abc" 3; "abcd" 4; "abcde" 5; "abcdef" 6; "abcdefg" 7; "abcdefgh" 8; "abcdefgh\r\n" 10; "abcdefgh\r\ni" 11; "abcdefgh\r\nij" 12; "abcdefgh\r\nij" 12
"a" 1; "ab" 2; "abc" 3; "abcd" 4; "abcde" 5; "abcdef" 6; "abcdefg" 7; "abcdefg\r\n" 9; "abcdefg\r\ni" 10; "abcdefg\r\nij" 11; "abcdefg\r\nij" 11
