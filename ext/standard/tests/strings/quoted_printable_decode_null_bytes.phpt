--TEST--
quoted_printable_decode() processes input after NUL bytes
--FILE--
<?php
$input = "abc\0d=41f";
$stream = fopen('php://temp', 'w+');
fwrite($stream, $input);
rewind($stream);
stream_filter_append($stream, 'convert.quoted-printable-decode', STREAM_FILTER_READ);
var_dump(quoted_printable_decode($input) === stream_get_contents($stream));
fclose($stream);

foreach ([
    "abc\0def",
    'A=00B',
    "A\0=42",
    "A=\0B",
    "A= \0B",
    "A=\r\n\0B",
    "A=0\0B",
    'A=',
    'A= ',
] as $input) {
    echo bin2hex(quoted_printable_decode($input)), "\n";
}
?>
--EXPECT--
bool(true)
61626300646566
410042
410042
413d0042
413d200042
410042
413d300042
41
41
