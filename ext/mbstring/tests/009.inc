<?php
// TODO: Add more encodings

//$debug=true;
ini_set('include_path','.');
include_once('common.inc');

// restore detect_order to 'auto'
mb_detect_order('auto');

// Test string
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。0123日本語は面倒臭い。';
$ascii  = 'abcdefghijklmnopqrstuvwxyz;]=#0123456789';

// ASCII
echo "== ASCII ==\n";
print  mb_strlen($ascii,'ASCII') . "\n";
print  mb_strlen($ascii) . "\n";

// EUC-JP
echo "== EUC-JP ==\n";
print  mb_strlen($euc_jp,'EUC-JP') . "\n";
mb_internal_encoding('EUC-JP') or print("mb_internal_encoding() failed\n");
print  mb_strlen($euc_jp) . "\n";

// SJIS
echo "== SJIS ==\n";
$sjis = mb_convert_encoding($euc_jp, 'SJIS');
print  mb_strlen($sjis,'SJIS') . "\n";
mb_internal_encoding('SJIS') or print("mb_internal_encoding() failed\n");
print  mb_strlen($sjis) . "\n";

// JIS
// Note: either convert_encoding or strlen has problem
echo "== JIS ==\n";
mb_internal_encoding('EUC-JP') or print("mb_internal_encoding() failed\n");
$jis = mb_convert_encoding($euc_jp, 'JIS');
print  mb_strlen($jis,'JIS') . "\n";
mb_internal_encoding('JIS')  or print("mb_internal_encoding() failed\n");
print  mb_strlen($jis) . "\n"; 

// UTF-8
// Note: either convert_encoding or strlen has problem
echo "== UTF-8 ==\n";
mb_internal_encoding('EUC-JP') or print("mb_internal_encoding() failed\n");
$utf8 = mb_convert_encoding($euc_jp, 'UTF-8');
print  mb_strlen($utf8,'UTF-8') . "\n";
mb_internal_encoding('UTF-8')  or print("mb_internal_encoding() failed\n");
print  mb_strlen($utf8) . "\n";  


// Wrong Parameters
echo "== WRONG PARAMETERS ==\n";
// Array
// Note: PHP Notice, but returns some value
$r = mb_strlen($t_ary);
echo $r."\n";
// Object
// Note: PHP Notice, but returns some value
$r = mb_strlen($t_obj);
echo $r."\n";
// Wrong encoding
mb_internal_encoding('EUC-JP');
$r = mb_strlen($euc_jp, 'BAD_NAME');
echo $r."\n";




?>
