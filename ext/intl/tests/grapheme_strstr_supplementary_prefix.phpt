--TEST--
grapheme_strstr() and grapheme_stristr() preserve match boundaries after supplementary characters
--EXTENSIONS--
intl
--FILE--
<?php
foreach (["\u{1F600}", "\u{1F600}\u{1F600}", "\u{1F600}e\u{0301}"] as $prefix) {
    $haystack = $prefix . 'abc';
    var_dump(grapheme_strstr($haystack, 'a'));
    var_dump(grapheme_strstr($haystack, 'a', true));
    var_dump(grapheme_stristr($haystack, 'A'));
    var_dump(grapheme_stristr($haystack, 'A', true));
}
?>
--EXPECT--
string(3) "abc"
string(4) "😀"
string(3) "abc"
string(4) "😀"
string(3) "abc"
string(8) "😀😀"
string(3) "abc"
string(8) "😀😀"
string(3) "abc"
string(7) "😀é"
string(3) "abc"
string(7) "😀é"
