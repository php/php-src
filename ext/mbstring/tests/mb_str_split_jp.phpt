--TEST--
mb_str_split() tests for the japanese language
--EXTENSIONS--
mbstring
--INI--
output_handler=
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

$string = "日本";             /* 2 chars */
$len = 2;
$charset = [
    "BIG-5",
    "EUC-JP",
    "ISO-2022-JP",
    "SJIS",
    "UTF-16BE",
    "UTF-16LE",
    "UTF-32BE",
    "UTF-32LE",
    "UTF-8"
];

/* Try empty strings first */
foreach ($charset as $cs) {
    if (count(mb_str_split("", 1, $cs)) !== 0)
        echo "Empty $cs string should convert to empty array!\n";
    if (count(mb_str_split("", 2, $cs)) !== 0)
        echo "Empty $cs string should convert to empty array!\n";
}

foreach ($charset as $cs) {
    $enc = mb_convert_encoding($string, $cs, "UTF-8");
    $split = mb_str_split($enc, 1, $cs);

    /* check chunks number */
    for($i = 1; $i <= $len; ++$i){
        $ceil = ceil($len / $i);
        $cnt = count(mb_str_split($enc,$i,$cs));
        if ($ceil != $cnt){
          echo "$cs WRONG CHUNKS NUMBER: expected/actual: $ceil/$cnt\n";
        }
    }

    /* check content */
    echo "$cs:";
    for($i = 0; $i < $len; ++$i){
        echo  " " . unpack("H*", $split[$i])[1];
    }
    echo "\n";
}

/* long string test */
$size = 50000;
$long = str_repeat($string, $size); /* 50k x 2 chars = 1e5 chars */
$enc = mb_convert_encoding($long, "ISO-2022-JP", "UTF-8");
$array = mb_str_split($enc, $len, "ISO-2022-JP");
$count = count($array);

/* check array size */
if($size !== $count) printf("Long string splitting error: actual array size: %d expected: %d\n", $count, $size);

/* compare initial string and last array element after splitting */
$enc = mb_convert_encoding($string, "ISO-2022-JP", "UTF-8");
if(end($array) !== $enc){
    printf("Long string splitting error:
        last array element: %s expected: %s\n", unpack("H*", end($array))[1],unpack("H*", $enc)[1]);
}

/* SJIS byte 0x80 was previously wrongly treated as the starting byte for a 2-byte character */
echo "== Regression test for SJIS byte 0x80 ==\n";
foreach (['SJIS', 'SJIS-2004', 'MacJapanese', 'SJIS-Mobile#DOCOMO', 'SJIS-Mobile#KDDI', 'SJIS-Mobile#SoftBank'] as $encoding) {
    $array = mb_str_split("\x80\xA1abc\x80\xA1", 2, $encoding);
    echo "$encoding: [" . implode(', ', array_map('bin2hex', $array)) . "]\n";

    // Also try bytes 0xFD, 0xFE, and 0xFF
    $array = mb_str_split("abc\xFD\xFE\xFFab\xFD\xFE\xFF", 2, $encoding);
    echo "$encoding: [" . implode(', ', array_map('bin2hex', $array)) . "]\n";
}

/*
Some MacJapanese characters map to a sequence of several Unicode codepoints. Examples:

0x85AB  0xF862+0x0058+0x0049+0x0049+0x0049  # roman numeral thirteen
0x85AC  0xF861+0x0058+0x0049+0x0056 # roman numeral fourteen
0x85AD  0xF860+0x0058+0x0056    # roman numeral fifteen
0x85BF  0xF862+0x0078+0x0069+0x0069+0x0069  # small roman numeral thirteen
0x85C0  0xF861+0x0078+0x0069+0x0076 # small roman numeral fourteen
0x85C1  0xF860+0x0078+0x0076    # small roman numeral fifteen

Even though they map to multiple codepoints, mb_str_split treats these as ONE character each
*/

echo "== MacJapanese characters which map to 3-5 codepoints each ==\n";
echo "[", implode(', ', array_map('bin2hex', mb_str_split("abc\x85\xAB\x85\xAC\x85\xAD", 1, 'MacJapanese'))), "]\n";
echo "[", implode(', ', array_map('bin2hex', mb_str_split("abc\x85\xBF\x85\xC0\x85\xC1", 2, 'MacJapanese'))), "]\n";

?>
--EXPECT--
BIG-5: a4e9 a5bb
EUC-JP: c6fc cbdc
ISO-2022-JP: 1b2442467c1b2842 1b24424b5c1b2842
SJIS: 93fa 967b
UTF-16BE: 65e5 672c
UTF-16LE: e565 2c67
UTF-32BE: 000065e5 0000672c
UTF-32LE: e5650000 2c670000
UTF-8: e697a5 e69cac
== Regression test for SJIS byte 0x80 ==
SJIS: [80a1, 6162, 6380, a1]
SJIS: [6162, 63fd, feff, 6162, fdfe, ff]
SJIS-2004: [80a1, 6162, 6380, a1]
SJIS-2004: [6162, 63fd, feff, 6162, fdfe, ff]
MacJapanese: [80a1, 6162, 6380, a1]
MacJapanese: [6162, 63fd, feff, 6162, fdfe, ff]
SJIS-Mobile#DOCOMO: [80a1, 6162, 6380, a1]
SJIS-Mobile#DOCOMO: [6162, 63fd, feff, 6162, fdfe, ff]
SJIS-Mobile#KDDI: [80a1, 6162, 6380, a1]
SJIS-Mobile#KDDI: [6162, 63fd, feff, 6162, fdfe, ff]
SJIS-Mobile#SoftBank: [80a1, 6162, 6380, a1]
SJIS-Mobile#SoftBank: [6162, 63fd, feff, 6162, fdfe, ff]
== MacJapanese characters which map to 3-5 codepoints each ==
[61, 62, 63, 85ab, 85ac, 85ad]
[6162, 6385bf, 85c085c1]
