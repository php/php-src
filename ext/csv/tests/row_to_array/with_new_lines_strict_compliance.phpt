--TEST--
Test Csv\row_to_array() with new lines (strict compliance version)
--DESCRIPTION--
RFC 4180 only allows CR and LF inside enclosed fields (TEXTDATA excludes them), which is also
how Csv\array_to_row() writes them. A CR or LF in a non escaped field that is not part of the
EOL sequence is an error, which also catches input using a different EOL sequence.
--EXTENSIONS--
csv
--FILE--
<?php

$fields = [
    "I have a\n new line",
    "I have a\r carriage return",
    "I use a carriage\r\n return new line",
    "I don't have a new line",
];

$enclosed = "\"I have a\n new line\",\"I have a\r carriage return\",\"I use a carriage\r\n return new line\",I don't have a new line\r\n";
var_dump($fields === Csv\row_to_array($enclosed));
var_dump($fields === Csv\row_to_array($enclosed, ','));
var_dump($fields === Csv\row_to_array($enclosed, ',', '"'));
var_dump($enclosed === Csv\array_to_row($fields));

foreach ([
    "I have a\n new line,b\r\n",
    "I have a\r carriage return,b\r\n",
    "a,b\nc,d\n",
    "a,b\r",
] as $row) {
    try {
        var_dump(Csv\row_to_array($row));
    } catch (\ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
}

/* With "\n" as the EOL sequence a CRLF terminated row leaves a stray CR */
try {
    var_dump(Csv\row_to_array("a,b\r\n", ',', '"', "\n"));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
ValueError: A non escaped field must not contain CR or LF characters that are not part of the EOL sequence
