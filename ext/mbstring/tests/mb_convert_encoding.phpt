--TEST--
mb_convert_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more tests
//$debug = true; // Uncomment this line to view error/warning/notice message in *.out file
ini_set('include_path','.');
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
$s = mb_convert_encoding($s, 'EUC-JP', 'SJIS');
print("EUC-JP: $s\n"); // EUC-JP

$s = $jis;
$s = mb_convert_encoding($s, 'EUC-JP', 'JIS');
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', 'EUC-JP');
print("SJIS: ".base64_encode($s)."\n"); // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', 'EUC-JP');
print("JIS: ".base64_encode($s)."\n"); // JIS


// Using Encoding List Array 
echo "== STRING ENCODING LIST ==\n";

$a = 'JIS,UTF-8,EUC-JP,SJIS';
$s = $jis;
$s = mb_convert_encoding($s, 'EUC-JP', $a);
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', $a);
print("SJIS: ".base64_encode($s)."\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', $a);
print("JIS: ".base64_encode($s)."\n"); // JIS


// Using Encoding List Array 
echo "== ARRAY ENCODING LIST ==\n";

$a = array(0=>'JIS', 1=>'UTF-8', 2=>'EUC-JP', 3=>'SJIS');
$s = $jis;
$s = mb_convert_encoding($s, 'EUC-JP', $a);
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', $a);
print("SJIS: ".base64_encode($s)."\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', $a);
print("JIS: ".base64_encode($s)."\n"); // JIS


// Using Detect Order 
echo "== DETECT ORDER ==\n";

$s = $jis;
$s = mb_convert_encoding($s, 'EUC-JP', 'auto');
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', 'auto');
print("SJIS: ".base64_encode($s)."\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', 'auto');
print("JIS: ".base64_encode($s)."\n"); // JIS


// Invalid(?) Parameters
echo "== INVALID PARAMETER ==\n";

$s = mb_convert_encoding(1234, 'EUC-JP');
print("INT: $s\n"); // EUC-JP

$s = mb_convert_encoding('', 'EUC-JP');
print("EUC-JP: $s\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'BAD');
print("BAD: $s\n"); // BAD

$s = $euc_jp;
$s = mb_convert_encoding($s);
print("MP: $s\n"); // Missing parameter


?>

--EXPECT--
== BASIC TEST ==
EUC-JP: 日本語テキストです。01234５６７８９。
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== STRING ENCODING LIST ==
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== ARRAY ENCODING LIST ==
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== DETECT ORDER ==
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== INVALID PARAMETER ==
INT: 1234
EUC-JP: 
ERR: Warning
BAD: 
ERR: Warning
MP: 


