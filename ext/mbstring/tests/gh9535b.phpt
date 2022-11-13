--TEST--
Output of mb_strcut covers requested range of bytes even when output contains ending escape sequences
--EXTENSIONS--
mbstring
--FILE--
<?php
// The existing behavior of mb_strcut is wrong for these encodings, when they add an extra closing
// escape sequence to a string which would otherwise end in a non-default conversion mode
// See https://github.com/php/php-src/pull/9562 for details on the bug

// These tests were developed when fixing a different bug, but they don't pass because of
// the bug involving the added closing escape sequences
// When that bug is fixed, we can remove XFAIL (or combine this file with gh9535.phpt)

$encodings = [
    'JIS',
    'ISO-2022-JP',
    'ISO-2022-JP-2004',
];

$input = '宛如繁星般宛如皎月般';
$bytes_length = 15;
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

$input = '星のように月のように';
$bytes_length = 20;
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

$input = 'あaいb';
$bytes_length = 10;
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

$input = 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';
$bytes_length = 10;
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

$input = '???';
$bytes_length = 2;
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

foreach($encodings as $encoding) {
    var_dump(mb_strcut($input, 0, $bytes_length, $encoding));
}

?>
--XFAIL--
Discussion: https://github.com/php/php-src/pull/9562
--EXPECTF--
JIS: 宛如繁星般
ISO-2022-JP: 宛如繁星般
ISO-2022-JP-2004: 宛如繁星

JIS: 星のように月の
ISO-2022-JP: 星のように月の
ISO-2022-JP-2004: 星のように月の

JIS: あa
ISO-2022-JP: あa
ISO-2022-JP-2004: あa

JIS: AAAAAAAAAA
ISO-2022-JP: AAAAAAAAAA
ISO-2022-JP-2004: AAAAAAAAAA

JIS: ??
ISO-2022-JP: ??
ISO-2022-JP-2004: ??

string(2) "??"
string(2) "??"
string(2) "??"
