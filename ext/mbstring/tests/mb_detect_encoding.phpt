--TEST--
mb_detect_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.language=Japanese
--FILE--
<?php
// TODO: Add more tests
//$debug = true; // Uncomment this line to view error/warning/notice message in *.out file
ini_set('include_path', dirname(__FILE__));
include_once('common.inc');

// SJIS string (BASE64 encoded)
$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
// JIS string (BASE64 encoded)
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
// EUC-JP string
$euc_jp = '日本語テキストです。01234５６７８９。';

// Test with sigle "form encoding"
// Note: For some reason it complains, results are differ. Not reserched.
echo "== BASIC TEST ==\n";
$s = $sjis;
$s = mb_detect_encoding($s, 'SJIS');
print("SJIS: $s\n");

$s = $jis;
$s = mb_detect_encoding($s, 'JIS');
print("JIS: $s\n");

$s = $euc_jp;
$s = mb_detect_encoding($s, 'UTF-8,EUC-JP,JIS');
print("EUC-JP: $s\n");

$s = $euc_jp;
$s = mb_detect_encoding($s, 'JIS,EUC-JP');
print("EUC-JP: $s\n");



// Using Encoding List Array
echo "== ARRAY ENCODING LIST ==\n";

$a = array(0=>'UTF-8',1=>'EUC-JP', 2=>'SJIS', 3=>'JIS');

// Note: Due to detect order, detected as UTF-8
$s = $jis;
$s = mb_detect_encoding($s, $a);
print("JIS: $s\n");

$s = $euc_jp;
$s = mb_detect_encoding($s, $a);
print("EUC-JP: $s\n");

$s = $sjis;
$s = mb_detect_encoding($s, $a);
print("SJIS: $s\n");


// Using Detect Order
echo "== DETECT ORDER ==\n";

mb_detect_order('auto');


$s = $jis;
$s = mb_detect_encoding($s);
print("JIS: $s\n");

$s = $euc_jp;
$s = mb_detect_encoding($s);
print("EUC-JP: $s\n");

$s = $sjis;
$s = mb_detect_encoding($s);
print("SJIS: $s\n");


// Invalid(?) Parameters
echo "== INVALID PARAMETER ==\n";

$s = mb_detect_encoding(1234, 'EUC-JP');
print("INT: $s\n"); // EUC-JP

$s = mb_detect_encoding('', 'EUC-JP');
print("EUC-JP: $s\n");  // SJIS

$s = $euc_jp;
$s = mb_detect_encoding($s, 'BAD');
print("BAD: $s\n"); // BAD

$s = $euc_jp;
$s = mb_detect_encoding();
print("MP: $s\n"); // Missing parameter


?>
--EXPECT--
== BASIC TEST ==
SJIS: SJIS
JIS: JIS
EUC-JP: EUC-JP
EUC-JP: EUC-JP
== ARRAY ENCODING LIST ==
JIS: UTF-8
EUC-JP: EUC-JP
SJIS: SJIS
== DETECT ORDER ==
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
== INVALID PARAMETER ==
INT: EUC-JP
EUC-JP: EUC-JP
ERR: Warning
BAD: EUC-JP
ERR: Warning
MP: 
