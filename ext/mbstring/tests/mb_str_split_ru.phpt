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
EUC-JP: a7e2a7d1a7db20a7e2a7d1a7db20a7e2a7d1a7db20
CP866: e0a0a920e0a0a920e0a0a920
KOI8-R: d2c1ca20d2c1ca20d2c1ca20
UTF-16BE: 044004300439002004400430043900200440043004390020
UTF-16LE: 400430043904200040043004390420004004300439042000
UTF-32BE: 000004400000043000000439000000200000044000000430000004390000002000000440000004300000043900000020
UTF-32LE: 400400003004000039040000200000004004000030040000390400002000000040040000300400003904000020000000
UTF-8: d180d0b0d0b920d180d0b0d0b920d180d0b0d0b920

