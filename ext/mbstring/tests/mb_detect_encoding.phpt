--TEST--
mb_detect_encoding()
--EXTENSIONS--
mbstring
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

$s = $jis;
$s = mb_detect_encoding($s, $a);
print("JIS: $s\n");

$s = $euc_jp;
$s = mb_detect_encoding($s, $a);
print("EUC-JP: $s\n");

$s = $sjis;
$s = mb_detect_encoding($s, $a);
print("SJIS: $s\n");

$test = "CHARSET=windows-1252:Do\xeb;John";
$encodings = ['UTF-8', 'SJIS', 'GB2312',
         'ISO-8859-1', 'ISO-8859-2', 'ISO-8859-3', 'ISO-8859-4',
         'ISO-8859-5', 'ISO-8859-6', 'ISO-8859-7', 'ISO-8859-8', 'ISO-8859-9',
         'ISO-8859-10', 'ISO-8859-13', 'ISO-8859-14', 'ISO-8859-15', 'ISO-8859-16',
         'WINDOWS-1252', 'WINDOWS-1251', 'EUC-JP', 'EUC-TW', 'KOI8-R', 'BIG-5',
         'ISO-2022-KR', 'ISO-2022-JP', 'UTF-16'
];
echo mb_detect_encoding($test, $encodings), "\n";

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
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
ISO-8859-1
== DETECT ORDER ==
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
== INVALID PARAMETER ==
INT: EUC-JP
EUC-JP: EUC-JP
mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "BAD"
