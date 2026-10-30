--TEST--
grapheme_strrpos() and grapheme_strripos() find overlapping matches with negative offsets
--EXTENSIONS--
intl
--FILE--
<?php
foreach (['a', "\u{00E9}", "e\u{0301}", "\u{1F600}"] as $grapheme) {
    foreach (['grapheme_strrpos', 'grapheme_strripos'] as $function) {
        $positions = [];
        foreach ([0, 1, -1, -2, -3] as $offset) {
            $positions[] = $function(str_repeat($grapheme, 3), str_repeat($grapheme, 2), $offset);
        }
        echo $function, ': ', implode(', ', $positions), "\n";
    }
}

var_dump(grapheme_strripos("\u{00C9}\u{00E9}\u{00C9}", "\u{00E9}\u{00E9}", -1));
var_dump(grapheme_strrpos("\u{00E9}\u{00E9}x\u{00E9}\u{00E9}", "\u{00E9}\u{00E9}", -1));
var_dump(grapheme_strrpos("\u{00E9}\u{00E9}x\u{00E9}\u{00E9}", "\u{00E9}\u{00E9}", -3));
?>
--EXPECT--
grapheme_strrpos: 1, 1, 1, 1, 0
grapheme_strripos: 1, 1, 1, 1, 0
grapheme_strrpos: 1, 1, 1, 1, 0
grapheme_strripos: 1, 1, 1, 1, 0
grapheme_strrpos: 1, 1, 1, 1, 0
grapheme_strripos: 1, 1, 1, 1, 0
grapheme_strrpos: 1, 1, 1, 1, 0
grapheme_strripos: 1, 1, 1, 1, 0
int(1)
int(3)
int(0)
