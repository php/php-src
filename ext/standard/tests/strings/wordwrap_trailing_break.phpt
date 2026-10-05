--TEST--
wordwrap() recognizes an existing break at the end of the string
--FILE--
<?php
$tests = [
    ["abc\n", 3, "\n", true],
    ["ab cd\n", 5, "\n", true],
    ["abcd\n", 3, "\n", true],
    ["abc\nabc\n", 3, "\n", true],
    ["abc-", 3, "-", true],
    ["abc\r\n", 3, "\r\n", true],
    ["\r\n", 1, "\r\n", true],
    ["ab cd\r\n", 5, "\r\n", false],
];

foreach ($tests as [$text, $width, $break, $cut]) {
    echo json_encode(wordwrap($text, $width, $break, $cut)), "\n";
}
?>
--EXPECT--
"abc\n"
"ab cd\n"
"abc\nd\n"
"abc\nabc\n"
"abc-"
"abc\r\n"
"\r\n"
"ab cd\r\n"
