--TEST--
Dialects in which some rows cannot be written unambiguously are rejected
--EXTENSIONS--
csv
--FILE--
<?php
$dialects = [
    /* A token starting with a token that the parser tries before it */
    ['--', '-', "\n"],
    [',', "\n", "\n\r"],
    ['|', '"', "|\n"],
    /* An enclosure that repeats the delimiter or the EOL sequence */
    ['xy', 'xyx', "\n"],
    ['-', '--', "\n"],
    [',', "\n\n", "\n"],
    /* A delimiter made of the EOL sequence followed by the enclosure */
    ["|'", "'", '|'],
    /* A self-overlapping enclosure whose occurrence straddling the closing one is followed by a token */
    ['ba', 'aba', "\n"],
    [',', 'aba', 'ba'],
    [',', 'aa', 'a;'],
];
foreach ($dialects as [$delimiter, $enclosure, $eol]) {
    try {
        var_dump(Csv\array_to_row(['a'], $delimiter, $enclosure, $eol));
    } catch (\ValueError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
}

/* Readers reject the same dialects */
try {
    var_dump(Csv\row_to_array("a\n", 'xy', 'xyx', "\n"));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(Csv\LazyLaxCollection::createFromBuffer("a\n", 'xy', 'xyx', "\n"));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

/* Overlapping tokens that can be told apart are still accepted */
var_dump(Csv\array_to_row(['a'], '--', 'aa', "\r\n"));
var_dump(Csv\array_to_row(['a'], '||', '"', '|'));
var_dump(Csv\array_to_row(['a'], ',', "\nX", "\n"));
?>
--EXPECT--
Csv\array_to_row(): Argument #2 ($delimiter) must not start with argument #3 ($enclosure)
Csv\array_to_row(): Argument #4 ($eolSequence) must not start with argument #3 ($enclosure)
Csv\array_to_row(): Argument #4 ($eolSequence) must not start with argument #2 ($delimiter)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #2 ($delimiter)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #2 ($delimiter)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #4 ($eolSequence)
Csv\array_to_row(): Argument #2 ($delimiter) is ambiguous with argument #3 ($enclosure)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #2 ($delimiter)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #4 ($eolSequence)
Csv\array_to_row(): Argument #3 ($enclosure) is ambiguous with argument #4 ($eolSequence)
Csv\row_to_array(): Argument #3 ($enclosure) is ambiguous with argument #2 ($delimiter)
Csv\LazyLaxCollection::createFromBuffer(): Argument #3 ($enclosure) is ambiguous with argument #2 ($delimiter)
string(3) "a
"
string(2) "a|"
string(2) "a
"
