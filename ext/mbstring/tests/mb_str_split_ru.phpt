--TEST--
mb_str_split()  tests for the russian language
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.func_overload=0
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

$string = "рай рай рай ";             /* 12 chars */
$len = 12;
$charset = [
    "EUC-JP",
    "CP866",
    "KOI8-R",
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
$size = 25000;
$long = str_repeat($string, $size); /* 25k x 12 chars = 3e5 chars */
$enc = mb_convert_encoding($long, "EUC-JP", "UTF-8");
$array = mb_str_split($enc, $len, "EUC-JP");
$count = count($array);

/* check array size */
if($size !== $count) printf("Long string splitting error: actual array size: %d expected: %d\n", $count, $size);

/* compare initial string and last array element after splitting */
$enc = mb_convert_encoding($string, "EUC-JP", "UTF-8");
if(end($array) !== $enc){
    printf("Long string splitting error:
        last array element: %s expected: %s\n", unpack("H*", end($array))[1],unpack("H*", $enc)[1]);
}

?>
--EXPECT--
EUC-JP: a7e2 a7d1 a7db 20 a7e2 a7d1 a7db 20 a7e2 a7d1 a7db 20
CP866: e0 a0 a9 20 e0 a0 a9 20 e0 a0 a9 20
KOI8-R: d2 c1 ca 20 d2 c1 ca 20 d2 c1 ca 20
UTF-16BE: 0440 0430 0439 0020 0440 0430 0439 0020 0440 0430 0439 0020
UTF-16LE: 4004 3004 3904 2000 4004 3004 3904 2000 4004 3004 3904 2000
UTF-32BE: 00000440 00000430 00000439 00000020 00000440 00000430 00000439 00000020 00000440 00000430 00000439 00000020
UTF-32LE: 40040000 30040000 39040000 20000000 40040000 30040000 39040000 20000000 40040000 30040000 39040000 20000000
UTF-8: d180 d0b0 d0b9 20 d180 d0b0 d0b9 20 d180 d0b0 d0b9 20
