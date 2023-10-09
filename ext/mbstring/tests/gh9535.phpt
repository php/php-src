--TEST--
GH-9535 (mb_strcut(): The behavior of mb_strcut in mbstring has been changed in PHP8.1)
--EXTENSIONS--
mbstring
--FILE--
<?php
$encodings = [
    'UTF-16',
    'UTF-16BE',
    'UTF-16LE',
    'UTF-7',
    'UTF7-IMAP',
    'ISO-2022-JP-MS',
    'GB18030',
    'HZ',
    'ISO-2022-KR',
    'ISO-2022-JP-MOBILE#KDDI',
    'CP50220',
    'CP50221',
    'CP50222',
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
// For ISO-2022-KR, the initial escape sequence 'ESC $ ) C' will occupy 4 bytes of the output;
// this will make mb_strcut only pick out 6 'A' characters from the input string and not 10
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.': '.$reconverted_str.PHP_EOL;
}

echo PHP_EOL;

$input = '???';
$bytes_length = 2;
// ISO-2022-KR will be affected by the initial escape sequence as stated above
foreach($encodings as $encoding) {
    $converted_str = mb_convert_encoding($input, $encoding, mb_internal_encoding());
    $cut_str = mb_strcut($converted_str, 0, $bytes_length, $encoding);
    $reconverted_str = mb_convert_encoding($cut_str, mb_internal_encoding(), $encoding);
    echo $encoding.trim(': '.$reconverted_str).PHP_EOL;
}

echo PHP_EOL;

foreach($encodings as $encoding) {
    echo $encoding.trim(': '.mb_strcut($input, 0, $bytes_length, $encoding)).PHP_EOL;
}

?>
--EXPECT--
UTF-16: 宛如繁星般宛如
UTF-16BE: 宛如繁星般宛如
UTF-16LE: 宛如繁星般宛如
UTF-7: 宛如繁星
UTF7-IMAP: 宛如繁星
ISO-2022-JP-MS: 宛如繁星
GB18030: 宛如繁星般宛如
HZ: 宛如繁星般
ISO-2022-KR: 宛如繁星
ISO-2022-JP-MOBILE#KDDI: 宛如繁星
CP50220: 宛如繁星
CP50221: 宛如繁星
CP50222: 宛如繁星

UTF-16: 星のように月のように
UTF-16BE: 星のように月のように
UTF-16LE: 星のように月のように
UTF-7: 星のように月
UTF7-IMAP: 星のように月
ISO-2022-JP-MS: 星のように月の
GB18030: 星のように月のように
HZ: 星のように月のよ
ISO-2022-KR: 星のように月の
ISO-2022-JP-MOBILE#KDDI: 星のように月の
CP50220: 星のように月の
CP50221: 星のように月の
CP50222: 星のように月の

UTF-16: あaいb
UTF-16BE: あaいb
UTF-16LE: あaいb
UTF-7: あa
UTF7-IMAP: あa
ISO-2022-JP-MS: あa
GB18030: あaいb
HZ: あa
ISO-2022-KR: あa
ISO-2022-JP-MOBILE#KDDI: あa
CP50220: あa
CP50221: あa
CP50222: あa

UTF-16: AAAAA
UTF-16BE: AAAAA
UTF-16LE: AAAAA
UTF-7: AAAAAAAAAA
UTF7-IMAP: AAAAAAAAAA
ISO-2022-JP-MS: AAAAAAAAAA
GB18030: AAAAAAAAAA
HZ: AAAAAAAAAA
ISO-2022-KR: AAAAAA
ISO-2022-JP-MOBILE#KDDI: AAAAAAAAAA
CP50220: AAAAAAAAAA
CP50221: AAAAAAAAAA
CP50222: AAAAAAAAAA

UTF-16: ?
UTF-16BE: ?
UTF-16LE: ?
UTF-7: ??
UTF7-IMAP: ??
ISO-2022-JP-MS: ??
GB18030: ??
HZ: ??
ISO-2022-KR:
ISO-2022-JP-MOBILE#KDDI: ??
CP50220: ??
CP50221: ??
CP50222: ??

UTF-16: ??
UTF-16BE: ??
UTF-16LE: ??
UTF-7: ??
UTF7-IMAP: ??
ISO-2022-JP-MS: ??
GB18030: ??
HZ: ??
ISO-2022-KR:
ISO-2022-JP-MOBILE#KDDI: ??
CP50220: ??
CP50221: ??
CP50222: ??
