--TEST--
Test Csv\buffer_to_collection(): input that does not follow RFC 4180 section 2 is rejected consistently
--EXTENSIONS--
csv
--FILE--
<?php
$cases = [
    'quote in a non escaped field' => "a\"b,c\r\n",
    'data after the closing quote' => "\"a\"b,c\r\n",
    'space after the closing quote' => "\"a\" ,b\r\n",
    'space before the opening quote' => " \"a\",b\r\n",
    'unterminated quote' => "\"a,b\r\nc,d\r\n",
    'unterminated quote, last row' => "a,b\r\n\"c,d",
    'LF only line endings' => "a,b\nc,d\n",
    'CR inside a non escaped field' => "a\rb,c\r\n",
    'BOM before a quoted field' => "\u{FEFF}\"a\",b\r\n",
    /* Valid input */
    'quoted fields' => "\"a\",\"b\"\r\n\"c\"\"d\",\"e\r\nf\"\r\n",
    'empty quoted field, no final EOL' => "\"\",b\r\nc,\"\"",
    'non-ASCII bytes' => "\u{e9},\u{fc}\r\n",
];
foreach ($cases as $name => $buffer) {
    echo $name, ': ';
    try {
        echo json_encode(Csv\buffer_to_collection($buffer)), \PHP_EOL;
    } catch (\ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
}
?>
--EXPECT--
quote in a non escaped field: ValueError: Enclosure sequence is used in a non escaped field
data after the closing quote: ValueError: Closing enclosure sequence must be followed by the delimiter or the EOL sequence
space after the closing quote: ValueError: Closing enclosure sequence must be followed by the delimiter or the EOL sequence
space before the opening quote: ValueError: Enclosure sequence is used in a non escaped field
unterminated quote: ValueError: Enclosure sequence is not closed
unterminated quote, last row: ValueError: Enclosure sequence is not closed
LF only line endings: ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
CR inside a non escaped field: ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
BOM before a quoted field: ValueError: Enclosure sequence is used in a non escaped field
quoted fields: [["a","b"],["c\"d","e\r\nf"]]
empty quoted field, no final EOL: [["","b"],["c",""]]
non-ASCII bytes: [["\u00e9","\u00fc"]]
