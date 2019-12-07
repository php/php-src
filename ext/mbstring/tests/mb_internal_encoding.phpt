--TEST--
mb_internal_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO:
ini_set('include_path', __DIR__);
include_once('common.inc');

// EUC-JP
$r = mb_internal_encoding('EUC-JP');
($r === TRUE) ? print "OK_EUC-JP_SET\n" : print "NG_EUC-JP_SET\n";
$enc = mb_internal_encoding();
print "$enc\n";

// UTF-8
$r = mb_internal_encoding('UTF-8');
($r === TRUE) ? print "OK_UTF-8_SET\n" : print "NG_UTF-8_SET\n";
$enc = mb_internal_encoding();
print "$enc\n";

// ASCII
$r = mb_internal_encoding('ASCII');
($r === TRUE) ? print "OK_ASCII_SET\n" : print "NG_ASCII_SET\n";
$enc = mb_internal_encoding();
print "$enc\n";

// Invalid Parameter
print "== INVALID PARAMETER ==\n";

// Note: Other than string type, PHP raises Warning

try {
    $r = mb_internal_encoding('BAD');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

($r === false) ? print "OK_BAD_SET\n" : print "NG_BAD_SET\n";
$enc = mb_internal_encoding();
print "$enc\n";

?>
--EXPECT--
OK_EUC-JP_SET
EUC-JP
OK_UTF-8_SET
UTF-8
OK_ASCII_SET
ASCII
== INVALID PARAMETER ==
Unknown encoding "BAD"
NG_BAD_SET
ASCII
