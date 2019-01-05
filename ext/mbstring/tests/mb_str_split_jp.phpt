--TEST--
mb_str_split() tests for the japanese language
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.func_overload=0
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

$string = "日本";             /* 2 chars */
$len = 2;
$charset = [
    "BIG-5", "EUC-JP", "ISO-2022-JP",
     "SJIS", "UTF-16BE", "UTF-16LE", "UTF-32BE",
      "UTF-32LE", "UTF-8"
];

$expected = [
    [
    "BIG-5" => "a4e9",
    "EUC-JP" => "c6fc",
    "ISO-2022-JP" => "1b2442467c1b2842",
    "SJIS" => "93fa",
    "UTF-16BE" => "65e5",
    "UTF-16LE" => "e565",
    "UTF-32BE" => "000065e5",
    "UTF-32LE" => "e5650000",
    "UTF-8" => "e697a5",
    ],
    [
    "BIG-5" => "a5bb",
    "EUC-JP" => "cbdc",
    "ISO-2022-JP" => "1b24424b5c1b2842",
    "SJIS" => "967b",
    "UTF-16BE" => "672c",
    "UTF-16LE" => "2c67",
    "UTF-32BE" => "0000672c",
    "UTF-32LE" => "2c670000",
    "UTF-8" => "e69cac",
    ],
];


foreach($charset as $cs){
    $enc = mb_convert_encoding($string, $cs, "UTF-8");
    $split = mb_str_split($enc, 1, $cs);

    /* check length */
    for($i = 1; $i <= $len; ++$i){
        $ceil = ceil($len / $i);
        if($ceil != count(mb_str_split($enc,$i,$cs))) echo "ERROR";
    }

    /* check content */
    echo "$cs: ";
    for($i = 0; $i < $len; ++$i){
        echo unpack("H*", $split[$i])[1];
    }
    echo "\n";
}


?>
--EXPECT--
BIG-5: a4e9a5bb
EUC-JP: c6fccbdc
ISO-2022-JP: 1b2442467c1b28421b24424b5c1b2842
SJIS: 93fa967b
UTF-16BE: 65e5672c
UTF-16LE: e5652c67
UTF-32BE: 000065e50000672c
UTF-32LE: e56500002c670000
UTF-8: e697a5e69cac

