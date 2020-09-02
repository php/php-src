--TEST--
mb_http_output()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
//TODO: Add more encoding. Wrong parameter type test.
ini_set('include_path', __DIR__);
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

// Note: Bad string raise ValueError. Bad Type raise Notice (Type Conversion) and ValueError
try {
    $r = mb_http_output('BAD_NAME');
    print 'NG_BAD_SET' . \PHP_EOL;
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
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
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "BAD_NAME" given
EUC-JP
