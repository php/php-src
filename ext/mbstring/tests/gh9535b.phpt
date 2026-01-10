--TEST--
Test output of mb_strcut for text encodings which use escape sequences
--EXTENSIONS--
mbstring
--FILE--
<?php

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
--EXPECTF--
JIS: 宛如繁星
ISO-2022-JP: 宛如繁星
ISO-2022-JP-2004: 宛如繁星

JIS: 星のように月の
ISO-2022-JP: 星のように月の
ISO-2022-JP-2004: 星のように月

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
