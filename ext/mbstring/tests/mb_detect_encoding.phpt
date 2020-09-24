--TEST--
mb_detect_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.language=Japanese
--FILE--
<?php
// TODO: Add more tests

// SJIS string (BASE64 encoded)
$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
// JIS string (BASE64 encoded)
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
// EUC-JP string
$euc_jp = '日本語テキストです。01234５６７８９。';

// Test with single "form encoding"
// Note: For some reason it complains, results are different. Not researched.
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
try {
    var_dump(mb_detect_encoding($s, 'BAD'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

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
mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "BAD"
