--TEST--
mb_internal_encoding()
--EXTENSIONS--
mbstring
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
    $r = mb_internal_encoding('BAD_NAME');
    print 'NG_BAD_SET' . \PHP_EOL;
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$enc = mb_internal_encoding();
print "$enc\n";

?>
--EXPECTF--

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
OK_EUC-JP_SET

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
EUC-JP

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
OK_UTF-8_SET

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
UTF-8

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
OK_ASCII_SET

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
ASCII
== INVALID PARAMETER ==

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
mb_internal_encoding(): Argument #1 ($encoding) must be a valid encoding, "BAD_NAME" given

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
ASCII
