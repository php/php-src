--TEST--
mb_http_output()  
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
//TODO: Add more encoding. Wrong parameter type test.
//$debug = true;
ini_set('include_path', dirname(__FILE__));
include_once('common.inc');

// Set HTTP output encoding to ASCII
$r = mb_http_output('ASCII');
($r === TRUE) ? print "OK_ASCII_SET\n" : print "NG_ASCII_SET\n";
$enc = mb_http_output();
print "$enc\n";

// Set HTTP output encoding to SJIS
$r = mb_http_output('SJIS');
($r === TRUE) ? print "OK_SJIS_SET\n" : print "NG_SJIS_SET\n";
$enc = mb_http_output();
print "$enc\n";

// Set HTTP output encoding to JIS
$r = mb_http_output('JIS');
($r === TRUE) ? print "OK_JIS_SET\n" : print "NG_JIS_SET\n";
$enc = mb_http_output();
print "$enc\n";

// Set HTTP output encoding to UTF8
$r = mb_http_output('UTF-8');
($r === TRUE) ? print "OK_UTF-8_SET\n" : print "NG_UTF-8_SET\n";
$enc = mb_http_output();
print "$enc\n";

// Set HTTP output encoding to EUC-JP
$r = mb_http_output('EUC-JP');
($r === TRUE) ? print "OK_EUC-JP_SET\n" : print "NG_EUC-JP_SET\n";
$enc = mb_http_output();
print "$enc\n";

// Invalid parameters
print "== INVALID PARAMETER ==\n";

// Note: Bad string raise Warning. Bad Type raise Notice (Type Conversion) and Warning....
$r = mb_http_output('BAD_NAME');
($r === FALSE) ? print "OK_BAD_SET\n" : print "NG_BAD_SET\n";
$enc = mb_http_output();
print "$enc\n";

$r = mb_http_output($t_ary);
($r === NULL) ? print "OK_BAD_ARY_SET\n" : print "NG_BAD_ARY_SET\n";
$enc = mb_http_output();
print "$enc\n";

$r = mb_http_output($t_obj);
($r === NULL) ? print "OK_BAD_OBJ_SET\n" : print "NG_BAD_OBJ_SET\n";
$enc = mb_http_output();
print "$enc\n";

?>

--EXPECT--
OK_ASCII_SET
ASCII
OK_SJIS_SET
SJIS
OK_JIS_SET
JIS
OK_UTF-8_SET
UTF-8
OK_EUC-JP_SET
EUC-JP
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_SET
EUC-JP
ERR: Warning
OK_BAD_ARY_SET
EUC-JP
ERR: Warning
OK_BAD_OBJ_SET
EUC-JP

