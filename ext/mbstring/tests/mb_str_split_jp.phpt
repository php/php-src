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


foreach($charset as $cs){
    $enc = mb_convert_encoding($string, $cs, "UTF-8");
    $split = mb_str_split($enc, 1, $cs);

    /* check chunks number */
    for($i = 1; $i <= $len; ++$i){
        $ceil = ceil($len / $i);
        $cnt = count(mb_str_split($enc,$i,$cs));
        if($ceil != $cnt){
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
