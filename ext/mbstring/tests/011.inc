<?php
// TODO: Add more encodings

//$debug=true;
ini_set('include_path','.');
include_once('common.inc');


// Test string
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。0123日本語は面倒臭い。';

// EUC-JP - With encoding parameter
mb_internal_encoding('UTF-8') or print("mb_internal_encoding() failed\n");

echo "== WITH ENCODING PARAMETER ==\n";

print  mb_strrpos($euc_jp,'日本語', 'EUC-JP') . "\n";
print  mb_strrpos($euc_jp, '0',     'EUC-JP') . "\n";
print  mb_strrpos($euc_jp, 3,       'EUC-JP') . "\n";
print  mb_strrpos($euc_jp, 0,       'EUC-JP') . "\n";


$r = mb_strrpos($euc_jp, '韓国語', 'EUC-JP');
($r === FALSE) ? print "OK_STR\n"     : print "NG_STR\n";
$r = mb_strrpos($euc_jp, "\n",     'EUC-JP');
($r === FALSE) ? print "OK_NEWLINE\n" : print "NG_NEWLINE\n";


// EUC-JP - No encoding parameter
echo "== NO ENCODING PARAMETER ==\n";
mb_internal_encoding('EUC-JP')  or print("mb_internal_encoding() failed\n");

print  mb_strrpos($euc_jp,'日本語') . "\n";
print  mb_strrpos($euc_jp, '0') . "\n";
print  mb_strrpos($euc_jp, 3) . "\n";
print  mb_strrpos($euc_jp, 0) . "\n";

$r = mb_strrpos($euc_jp,'韓国語');
($r === FALSE) ? print "OK_STR\n"     : print "NG_STR\n";
$r = mb_strrpos($euc_jp,"\n");
($r === FALSE) ? print "OK_NEWLINE\n" : print "NG_NEWLINE\n";


// Invalid Parameters
echo "== INVALID PARAMETER TEST ==\n";

$r = mb_strrpos($euc_jp,'','EUC-JP');
($r === FALSE) ? print("OK_NULL\n") : print("NG_NULL\n");
$r = mb_strrpos($euc_jp, $t_ary, 'EUC-JP');
($r === FALSE) ? print("OK_ARRAY\n") : print("NG_ARRAY\n");
$r = mb_strrpos($euc_jp, $t_obj, 'EUC-JP');
($r === FALSE) ? print("OK_OBJECT\n") : print("NG_OBJECT\n");

//Note: Notice msg for NULL string might be better.


?>
