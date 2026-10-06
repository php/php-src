--TEST--
grapheme_extract() includes skipped UTF-8 continuation bytes in the next offset
--EXTENSIONS--
intl
--FILE--
<?php

$cases = [
    ["\u{00E9}x", 1, 1, GRAPHEME_EXTR_COUNT],
    ["\u{1F600}x", 1, 1, GRAPHEME_EXTR_COUNT],
    ["\u{1F600}x", 2, 1, GRAPHEME_EXTR_COUNT],
    ["\u{1F600}x", 3, 1, GRAPHEME_EXTR_COUNT],
    ["\u{00E9}\u{1F600}x", 1, 1, GRAPHEME_EXTR_COUNT],
    ["\u{00E9}\u{1F600}x", 1, 4, GRAPHEME_EXTR_MAXBYTES],
    ["\u{00E9}\u{1F600}x", 1, 1, GRAPHEME_EXTR_MAXCHARS],
    ["\u{1F600}\u{00E9}x", -4, 1, GRAPHEME_EXTR_COUNT],
    ["\u{00E9}x", -2, 1, GRAPHEME_EXTR_COUNT],
    ["\u{1F600}x", 0, 1, GRAPHEME_EXTR_COUNT],
    ["\u{00E9}x", 1, 0, GRAPHEME_EXTR_COUNT],
];

foreach ($cases as [$string, $start, $size, $type]) {
    $result = grapheme_extract($string, $size, $type, $start, $next);
    echo bin2hex($result), ' ', $next, "\n";
}

?>
--EXPECT--
78 3
78 5
78 5
78 5
f09f9880 6
f09f9880 6
f09f9880 6
c3a9 6
78 3
f09f9880 4
 1
