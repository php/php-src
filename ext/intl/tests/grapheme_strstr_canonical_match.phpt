--TEST--
grapheme_strstr() finds canonically equivalent strings with different byte representations
--EXTENSIONS--
intl
--FILE--
<?php
foreach ([
    ["e\u{0301}x", "\u{00E9}"],
    ["\u{00E9}x", "e\u{0301}"],
    ["pe\u{0301}x", "\u{00E9}"],
    ["pe\u{0301}x\u{00E9}", "\u{00E9}"],
    ['abc', 'b'],
    ['abc', 'z'],
] as [$haystack, $needle]) {
    echo json_encode([
        grapheme_strstr($haystack, $needle),
        grapheme_strstr($haystack, $needle, true),
    ]), "\n";
}
?>
--EXPECT--
["e\u0301x",""]
["\u00e9x",""]
["e\u0301x","p"]
["e\u0301x\u00e9","p"]
["bc","a"]
[false,false]
