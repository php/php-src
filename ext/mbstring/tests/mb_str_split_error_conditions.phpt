--TEST--
mb_str_split() error conditions
--EXTENSIONS--
mbstring
--FILE--
<?php

$string = "日本"; /* 2 chars */

// Invalid split length
try {
    mb_str_split($string, 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mb_str_split($string, -5);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

//Invalid Encoding
try {
    mb_str_split($string, 1, "BAD_ENCODING");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// For UTF-8, error markers are not inserted
echo "== INVALID UTF-8 ==\n";
$array = mb_str_split("abc\xFFabc", 2, "UTF-8");
echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";

// For most other encodings, they are
echo "== INVALID HZ ==\n";
// The last string emitted by mb_str_split will include '?' as an error marker,
// since ά cannot be represented in HZ
$array = mb_str_split(mb_convert_encoding("ελληνικά", "HZ", "UTF-8"), 2, "HZ");
echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";

// HTML entity error markers
mb_substitute_character("entity");
echo "== INVALID HZ IN 'ENTITY' ERROR OUTPUT MODE ==\n";
// The output here will actually include an HTML entity #x3AC;
// It will be split into segments of 2 characters each by mb_str_split
$array = mb_str_split(mb_convert_encoding("ελληνικά", "HZ", "UTF-8"), 2, "HZ");
echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";

?>
--EXPECT--
mb_str_split(): Argument #2 ($length) must be greater than 0
mb_str_split(): Argument #2 ($length) must be greater than 0
mb_str_split(): Argument #3 ($encoding) must be a valid encoding, "BAD_ENCODING" given
== INVALID UTF-8 ==
[6162, 63ff, 6162, 63]
== INVALID HZ ==
[7e7b2645264b7e7d, 7e7b264b26477e7d, 7e7b264d26497e7d, 7e7b264a7e7d3f]
== INVALID HZ IN 'ENTITY' ERROR OUTPUT MODE ==
[7e7b2645264b7e7d, 7e7b264b26477e7d, 7e7b264d26497e7d, 7e7b264a7e7d26, 2378, 3341, 433b]
