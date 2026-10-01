--TEST--
grapheme_strpos() uses Unicode matching for non-ASCII needles in ASCII haystacks
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=1
intl.error_level=2
--FILE--
<?php
foreach ([
    ['K', "\u{212A}", 0],
    ['aKbK', "\u{212A}", 0],
    ['aKbK', "\u{212A}", 1],
    ['aKbK', "\u{212A}", 2],
    ['aKbK', "\u{212A}", 3],
    ['aKbK', "\u{212A}", 4],
    ['aKbK', "\u{212A}", -4],
    ['aKbK', "\u{212A}", -2],
    ['abc', "b\u{00AD}", 0],
    ['abc', "\u{00E9}", 0],
    ['abc', 'b', 1],
    ['abc', 'b', 2],
    ['', "\u{00E9}", 0],
    ['', "e\u{0301}", 0],
    ['', "\r\n", 0],
    ['', '', 0],
    ['abc', '', 3],
] as [$haystack, $needle, $offset]) {
    var_dump(grapheme_strpos($haystack, $needle, $offset));
}

try {
    grapheme_strpos('', "\u{212A}", 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(1)
int(1)
int(3)
int(3)
bool(false)
int(1)
int(3)
int(1)
bool(false)
int(1)
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
int(3)
grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
