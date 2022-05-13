--TEST--
mb_convert_encoding()
--EXTENSIONS--
mbstring
--INI--
output_handler=
mbstring.language=Japanese
--FILE--
<?php
// TODO: Add more tests

$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
$euc_jp = "\xC6\xFC\xCB\xDC\xB8\xEC\xA5\xC6\xA5\xAD\xA5\xB9\xA5\xC8\xA4\xC7\xA4\xB9\xA1\xA301234\xA3\xB5\xA3\xB6\xA3\xB7\xA3\xB8\xA3\xB9\xA1\xA3";

// Test with single "form encoding"
echo "== BASIC TEST ==\n";
$s = bin2hex(mb_convert_encoding($sjis, 'EUC-JP', 'SJIS'));
print("EUC-JP: $s\n"); // EUC-JP

$s = bin2hex(mb_convert_encoding($jis, 'EUC-JP', 'JIS'));
print("EUC-JP: $s\n"); // EUC-JP

$s = mb_convert_encoding($euc_jp, 'SJIS', 'EUC-JP');
print("SJIS: ".base64_encode($s)."\n"); // SJIS

$s = mb_convert_encoding($euc_jp, 'JIS', 'EUC-JP');
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

$a = ['JIS', 'UTF-8', 'EUC-JP', 'SJIS'];
$s = $jis;
$s = bin2hex(mb_convert_encoding($s, 'EUC-JP', $a));
print("EUC-JP: $s\n"); // EUC-JP

$s = $euc_jp;
$s = mb_convert_encoding($s, 'SJIS', $a);
print("SJIS: ".base64_encode($s)."\n");  // SJIS

$s = $euc_jp;
$s = mb_convert_encoding($s, 'JIS', $a);
print("JIS: ".base64_encode($s)."\n"); // JIS

// Regression test for bug #81676
echo "UTF-8: " . mb_convert_encoding('test', 'UTF-8', mb_list_encodings()), "\n";

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

echo "== INVALID PARAMETER ==\n";

$s = mb_convert_encoding(1234, 'EUC-JP');
print("INT: $s\n");

$s = mb_convert_encoding('', 'EUC-JP');
print("EUC-JP: $s\n");  // SJIS

function tryBadConversion($str, $encoding) {
    try {
        var_dump(mb_convert_encoding($str, $encoding));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

tryBadConversion($euc_jp, 'BAD');

tryBadConversion('abc', 'Quoted-Printable');
tryBadConversion('abc', 'BASE64');
tryBadConversion('abc', 'HTML-ENTITIES');

?>
--EXPECTF--
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
UTF-8: test
== DETECT ORDER ==
EUC-JP: c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== INVALID PARAMETER ==
INT: 1234
EUC-JP: 
mb_convert_encoding(): Argument #2 ($to_encoding) must be a valid encoding, "BAD" given

Deprecated: mb_convert_encoding(): Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead in %s on line %d
string(3) "abc"

Deprecated: mb_convert_encoding(): Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead in %s on line %d
string(4) "YWJj"

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s on line %d
string(3) "abc"
