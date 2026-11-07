--TEST--
utf8_decode() with valid and invalid UTF-8 sequences around the two-, three- and four-byte boundaries
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cases = [
    'C2 80, smallest two-byte' => "\xC2\x80",
    'C3 A9, Latin-1' => "\xC3\xA9",
    'C3 BF, largest Latin-1' => "\xC3\xBF",
    'C4 80, above Latin-1' => "\xC4\x80",
    'DF BF, largest two-byte' => "\xDF\xBF",
    'C0 AF, overlong' => "\xC0\xAF",
    'C1 BF, overlong' => "\xC1\xBF",
    '80, lone continuation byte' => "\x80",
    'C2 at the end' => "a\xC2",
    'C2 then ASCII' => "\xC2a",
    'C2 then lead byte' => "\xC2\xC3\xA9",
    'E0 A0 80, smallest three-byte' => "\xE0\xA0\x80",
    'E0 9F BF, overlong' => "\xE0\x9F\xBF",
    'E0 83 A9, overlong' => "\xE0\x83\xA9",
    'ED 9F BF, before surrogates' => "\xED\x9F\xBF",
    'ED A0 80, surrogate' => "\xED\xA0\x80",
    'ED BF BF, surrogate' => "\xED\xBF\xBF",
    'ED A0 at the end' => "a\xED\xA0",
    'EE 80 80, after surrogates' => "\xEE\x80\x80",
    'EF BF BF, largest three-byte' => "\xEF\xBF\xBF",
    'E2 82 at the end' => "a\xE2\x82",
    'E2 82 then ASCII' => "\xE2\x82A",
    'E2 then ASCII' => "\xE2ab",
    'F0 90 80 80, smallest four-byte' => "\xF0\x90\x80\x80",
    'F4 8F BF BF, largest four-byte' => "\xF4\x8F\xBF\xBF",
    'F0 8F BF BF, overlong' => "\xF0\x8F\xBF\xBF",
    'F0 80 83 A9, overlong' => "\xF0\x80\x83\xA9",
    'F4 90 80 80, above U+10FFFF' => "\xF4\x90\x80\x80",
    'F5 80 80 80' => "\xF5\x80\x80\x80",
    'F0 9F 98 at the end' => "a\xF0\x9F\x98",
    'F0 9F 98 then ASCII' => "\xF0\x9F\x98a",
    'F0 9F 98 then lead byte' => "\xF0\x9F\x98\xF0\x9F\x98\x80",
    'four-byte between ASCII' => "x\xF0\x9F\x98\x80y",
    'two-byte between ASCII' => "x\xC3\xA9y",
    'three-byte between ASCII' => "x\xE6\xBC\xA2y",
];
foreach ($cases as $name => $str) {
    echo "$name: ", bin2hex(utf8_decode($str)), "\n";
}
?>
--EXPECT--
C2 80, smallest two-byte: 80
C3 A9, Latin-1: e9
C3 BF, largest Latin-1: ff
C4 80, above Latin-1: 3f
DF BF, largest two-byte: 3f
C0 AF, overlong: 3f3f
C1 BF, overlong: 3f3f
80, lone continuation byte: 3f
C2 at the end: 613f
C2 then ASCII: 3f61
C2 then lead byte: 3fe9
E0 A0 80, smallest three-byte: 3f
E0 9F BF, overlong: 3f
E0 83 A9, overlong: 3f
ED 9F BF, before surrogates: 3f
ED A0 80, surrogate: 3f
ED BF BF, surrogate: 3f
ED A0 at the end: 613f
EE 80 80, after surrogates: 3f
EF BF BF, largest three-byte: 3f
E2 82 at the end: 613f
E2 82 then ASCII: 3f41
E2 then ASCII: 3f6162
F0 90 80 80, smallest four-byte: 3f
F4 8F BF BF, largest four-byte: 3f
F0 8F BF BF, overlong: 3f
F0 80 83 A9, overlong: 3f
F4 90 80 80, above U+10FFFF: 3f
F5 80 80 80: 3f3f3f3f
F0 9F 98 at the end: 613f
F0 9F 98 then ASCII: 3f61
F0 9F 98 then lead byte: 3f3f
four-byte between ASCII: 783f79
two-byte between ASCII: 78e979
three-byte between ASCII: 783f79
