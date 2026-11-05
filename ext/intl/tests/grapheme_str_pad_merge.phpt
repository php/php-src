--TEST--
grapheme_str_pad() with padding or input fragments that merge at a join
--EXTENSIONS--
intl
--FILE--
<?php
// $string and each copy of $pad_string are measured on their own. A fragment that
// attaches to its neighbour merges at the join, so the result has fewer grapheme
// clusters than $length, as in any concatenation.
$cases = [
    'padding starting with a combining mark' => ["e", 3, "\u{0301}", STR_PAD_RIGHT, "e\u{0301}\u{0301}"],
    'padding starting with U+200D ZWJ' => ["\u{1F468}", 3, "\u{200D}", STR_PAD_RIGHT, "\u{1F468}\u{200D}\u{200D}"],
    'lone regional indicator' => ["", 2, "\u{1F1EA}", STR_PAD_RIGHT, "\u{1F1EA}\u{1F1EA}"],
    'Hangul conjoining jamo' => ["\u{1100}", 3, "\u{1161}", STR_PAD_RIGHT, "\u{1100}\u{1161}\u{1161}"],
    'string starting with a combining mark' => ["\u{0301}x", 4, "a", STR_PAD_LEFT, "aa\u{0301}x"],
    '"\r" padding before a "\n"' => ["\nx", 4, "\r", STR_PAD_LEFT, "\r\r\nx"],
    'string ending with "\r" before "\n" padding' => ["x\r", 4, "\n", STR_PAD_RIGHT, "x\r\n\n"],
];

foreach ($cases as $name => [$string, $length, $pad, $type, $expected]) {
    $result = grapheme_str_pad($string, $length, $pad, $type);
    echo $name, ': ', var_export($result === $expected, true), ', ', grapheme_strlen($result), ' of ', $length, PHP_EOL;
}
?>
--EXPECT--
padding starting with a combining mark: true, 1 of 3
padding starting with U+200D ZWJ: true, 1 of 3
lone regional indicator: true, 1 of 2
Hangul conjoining jamo: true, 1 of 3
string starting with a combining mark: true, 3 of 4
"\r" padding before a "\n": true, 3 of 4
string ending with "\r" before "\n" padding: true, 3 of 4
