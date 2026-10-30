--TEST--
mb_stripos()
--EXTENSIONS--
mbstring
--FILE--
<?php
// TODO: Add more encodings

ini_set('include_path','.');
include_once('common.inc');

// Test string
$euc_jp = "0123\xA4\xB3\xA4\xCE\xCA\xB8\xBB\xFA\xCE\xF3\xA4\xCF\xC6\xFC\xCB\xDC\xB8\xEC\xA4\xC7\xA4\xB9\xA1\xA3EUC-JP\xA4\xF2\xBB\xC8\xA4\xC3\xA4\xC6\xA4\xA4\xA4\xDE\xA4\xB9\xA1\xA30123\xC6\xFC\xCB\xDC\xB8\xEC\xA4\xCF\xCC\xCC\xC5\xDD\xBD\xAD\xA4\xA4\xA1\xA3";

$slen = mb_strlen($euc_jp, 'EUC-JP');
echo "String len: $slen\n";

// EUC-JP - With encoding parameter
mb_internal_encoding('UTF-8') or print("mb_internal_encoding() failed\n");

echo  "== POSITIVE OFFSET ==\n";

print  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", 0, 'EUC-JP') . "\n";
print  mb_stripos($euc_jp, '0', 0,     'EUC-JP') . "\n";
print  mb_stripos($euc_jp, 3, 0,       'EUC-JP') . "\n";
print  mb_stripos($euc_jp, 0, 0,       'EUC-JP') . "\n";
print  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", 15, 'EUC-JP') . "\n";
print  mb_stripos($euc_jp, '0', 15,     'EUC-JP') . "\n";
print  mb_stripos($euc_jp, 3, 15,       'EUC-JP') . "\n";
print  mb_stripos($euc_jp, 0, 15,       'EUC-JP') . "\n";


// Negative offset
echo "== NEGATIVE OFFSET ==\n";

print mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", -15, 'EUC-JP') . "\n";
print mb_stripos($euc_jp, '0', -15,     'EUC-JP') . "\n";
print mb_stripos($euc_jp, 3, -15,       'EUC-JP') . "\n";
print mb_stripos($euc_jp, 0, -15,       'EUC-JP') . "\n";
print mb_stripos($euc_jp, 0, -43,       'EUC-JP') . "\n";


// Out of range - should return false
print "== OUT OF RANGE ==\n";

$r =  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", 40, 'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =  mb_stripos($euc_jp, '0', 40,     'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =  mb_stripos($euc_jp, 3, 40,       'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =   mb_stripos($euc_jp, 0, 40,       'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", -3, 'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =  mb_stripos($euc_jp, '0', -3,     'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =  mb_stripos($euc_jp, 3, -3,       'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";
$r =   mb_stripos($euc_jp, 0, -3,       'EUC-JP');
($r === FALSE) ? print "OK_OUT_RANGE\n"     : print "NG_OUT_RANGE\n";


// Non-existent
echo "== NON-EXISTENT ==\n";

$r = mb_stripos($euc_jp, "\xB4\xDA\xB9\xF1\xB8\xEC", 0, 'EUC-JP');
($r === FALSE) ? print "OK_STR\n"     : print "NG_STR\n";
$r = mb_stripos($euc_jp, "\n",     0, 'EUC-JP');
($r === FALSE) ? print "OK_NEWLINE\n" : print "NG_NEWLINE\n";


// EUC-JP - No encoding parameter
echo "== NO ENCODING PARAMETER ==\n";

mb_internal_encoding('EUC-JP')  or print("mb_internal_encoding() failed\n");

print  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC", 0) . "\n";
print  mb_stripos($euc_jp, '0', 0) . "\n";
print  mb_stripos($euc_jp, 3, 0) . "\n";
print  mb_stripos($euc_jp, 0, 0) . "\n";

$r = mb_stripos($euc_jp, "\xB4\xDA\xB9\xF1\xB8\xEC", 0);
($r === FALSE) ? print "OK_STR\n"     : print "NG_STR\n";
$r = mb_stripos($euc_jp, "\n", 0);
($r === FALSE) ? print "OK_NEWLINE\n" : print "NG_NEWLINE\n";

// EUC-JP - No offset and encoding parameter
echo "== NO OFFSET AND ENCODING PARAMETER ==\n";

mb_internal_encoding('EUC-JP')  or print("mb_internal_encoding() failed\n");

print  mb_stripos($euc_jp, "\xC6\xFC\xCB\xDC\xB8\xEC") . "\n";
print  mb_stripos($euc_jp, '0') . "\n";
print  mb_stripos($euc_jp, 3) . "\n";
print  mb_stripos($euc_jp, 0) . "\n";

$r = mb_stripos($euc_jp, "\xB4\xDA\xB9\xF1\xB8\xEC");
($r === FALSE) ? print "OK_STR\n"     : print "NG_STR\n";
$r = mb_stripos($euc_jp, "\n");
($r === FALSE) ? print "OK_NEWLINE\n" : print "NG_NEWLINE\n";

echo "== INVALID STRINGS ==\n";

// Previously, mb_stripos would internally convert invalid byte sequences to the
// mb_substitute_char BEFORE performing search
// So invalid byte sequences would match the substitute char, both from haystack
// to needle and needle to haystack

mb_substitute_character(0x25); // '%'
var_dump(mb_stripos("abc%%", "\xFF", 0, "UTF-8")); // should be false
var_dump(mb_stripos("abc\xFF", "%", 0, "UTF-8")); // should be false

// However, invalid byte sequences can still match invalid byte sequences:
var_dump(mb_stripos("abc\x80\x80", "\xFF", 0, "UTF-8"));
var_dump(mb_stripos("abc\xFF", "c\x80", 0, "UTF-8"));


?>
--EXPECT--
String len: 43
== POSITIVE OFFSET ==
10
0
3
0
34
30
33
30
== NEGATIVE OFFSET ==
34
30
33
30
0
== OUT OF RANGE ==
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
OK_OUT_RANGE
== NON-EXISTENT ==
OK_STR
OK_NEWLINE
== NO ENCODING PARAMETER ==
10
0
3
0
OK_STR
OK_NEWLINE
== NO OFFSET AND ENCODING PARAMETER ==
10
0
3
0
OK_STR
OK_NEWLINE
== INVALID STRINGS ==
bool(false)
bool(false)
int(3)
int(2)
