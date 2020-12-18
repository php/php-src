--TEST--
mb_convert_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.language=Japanese
--FILE--
<?php
// TODO: Add more tests

// SJIS string (BASE64 encoded)
$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
// JIS string (BASE64 encoded)
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
// EUC-JP string
$euc_jp = '���ܸ�ƥ����ȤǤ���01234������������';

// Test with single "form encoding"
// Note: For some reason it complains, results are different. Not researched.
echo "== BASIC TEST ==\n";
$s = $sjis;
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', 'SJIS'));
print("EUC-JP: $s\n"); // EUC-JP

$s = $jis;
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', 'JIS'));
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
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', $a));
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
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', $a));
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
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', 'auto'));
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', 'auto');
print("SJIS: ".base64_encode($s)."\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', 'auto');
print("JIS: ".base64_encode($s)."\n"); // JIS


// Invalid Parameters
echo "== INVALID PARAMETER ==\n";

$s = mb_convert_encoding(1234, 'EUC-JP');
print("INT: $s\n");

$s = mb_convert_encoding('', 'EUC-JP');
print("EUC-JP: $s\n");  // SJIS

$s = $euc_jp;
try {
    var_dump(mb_convert_encoding($s, 'BAD'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
== BASIC TEST ==
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== STRING ENCODING LIST ==
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== ARRAY ENCODING LIST ==
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== DETECT ORDER ==
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== INVALID PARAMETER ==
INT: 1234
EUC-JP: 
mb_convert_encoding(): Argument #2 ($to_encoding) must be a valid encoding, "BAD" given
