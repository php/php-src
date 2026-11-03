--TEST--
grapheme_str_pad() pads to a length counted in grapheme clusters
--EXTENSIONS--
intl
--FILE--
<?php
// ASCII behaves like str_pad()
var_dump(grapheme_str_pad("abc", 6, "-") === str_pad("abc", 6, "-"));
var_dump(grapheme_str_pad("abc", 7, "xy", STR_PAD_LEFT) === str_pad("abc", 7, "xy", STR_PAD_LEFT));
var_dump(grapheme_str_pad("abc", 8, "xy", STR_PAD_BOTH) === str_pad("abc", 8, "xy", STR_PAD_BOTH));

// NFC and NFD forms of the same text get the same padding
$nfc = "M\u{00FC}ller";
$nfd = "Mu\u{0308}ller";
var_dump(grapheme_str_pad($nfc, 8, "."));
var_dump(grapheme_str_pad($nfd, 8, ".") === $nfd . "..");

// Japanese with combining dakuten, Devanagari, emoji with modifier and ZWJ sequence
var_dump(grapheme_str_pad("\u{304B}\u{3099}\u{304D}\u{3099}", 4, "*") === "\u{304B}\u{3099}\u{304D}\u{3099}**");
var_dump(grapheme_strlen(grapheme_str_pad("नमस्ते", 5, "-")));
var_dump(grapheme_str_pad("👍🏽", 3, "-", STR_PAD_LEFT) === "--👍🏽");
var_dump(grapheme_str_pad("x", 4, "👨‍👩‍👧", STR_PAD_RIGHT) === "x👨‍👩‍👧👨‍👩‍👧👨‍👩‍👧");

// The padding string is cut on a grapheme boundary
var_dump(grapheme_str_pad("a", 4, "u\u{0308}o\u{0308}") === "au\u{0308}o\u{0308}u\u{0308}");
var_dump(grapheme_str_pad("ab", 5, "\u{304B}\u{3099}x", STR_PAD_BOTH) === "\u{304B}\u{3099}ab\u{304B}\u{3099}x");

// "\r\n" is a single grapheme
var_dump(grapheme_str_pad("\r\n", 2, "-") === "\r\n-");

// Nothing to pad
var_dump(grapheme_str_pad("abc", 3, "-"));
var_dump(grapheme_str_pad("abc", -1, "-"));
var_dump(grapheme_str_pad("", 3));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(9) "Müller.."
bool(true)
bool(true)
int(5)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(3) "abc"
string(3) "abc"
string(3) "   "
