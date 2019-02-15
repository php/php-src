--TEST--
mb_convert_variables()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
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

// Test for single scaler
echo "== SCALER TEST ==\n";
$s = $sjis;
$encoding = mb_convert_variables('EUC-JP', 'SJIS', $s);
print("$encoding\n"); // SJIS
print("$s\n"); // Converted to EUC-JP

$s = $jis;
$encoding = mb_convert_variables('EUC-JP', 'JIS', $s);
print("$encoding\n"); // JIS
print("$s\n"); // Converted to EUC-JP

$s = $euc_jp;
$encoding = mb_convert_variables('SJIS', 'EUC-JP', $s);
print("$encoding\n"); // EUC-JP
print(base64_encode($s)."\n"); // Converted to SJIS (base64 encoded)

$s = $euc_jp;
$encoding = mb_convert_variables('JIS', 'EUC-JP', $s);
print("$encoding\n"); // EUC-JP
print(base64_encode($s)."\n"); // Converted to JIS (base64 encoded)

// Test for multiple slcaler
$s1 = $euc_jp;
$s2 = $euc_jp;
$s3 = $euc_jp;
$encoding = mb_convert_variables('EUC-JP', 'auto', $s1, $s2, $s3);
print("$encoding\n"); // EUC-JP
print("$s1$s2$s3\n"); // Converted to EUC-JP



// Note: Mixing encoding in array/object is not supported?
// Test for array
echo "== ARRAY TEST ==\n";
$a = array($s3, $s2, $s1);
$aa = $a;
$encoding = mb_convert_variables('EUC-JP', 'auto', $aa);
print("$encoding\n"); // EUC-JP
print("{$aa[0]}{$aa[1]}{$aa[2]}\n"); // Converted to EUC-JP

$a = array($s1, $s2, $s3);
$aa = $a;
$encoding = mb_convert_variables('EUC-JP', 'auto', $aa);
print("$encoding\n"); // EUC-JP
print("{$aa[0]}{$aa[1]}{$aa[2]}\n"); // Converted to EUC-JP



// Test for object
echo "== OBJECT TEST ==\n";
class foo
{
	public $s1;
	public $s2;
	public $s3;

	function __construct()
	{
		global $sjis, $jis, $euc_jp;

		$this->s1 = $euc_jp;
		$this->s2 = $euc_jp;
		$this->s3 = $euc_jp;
	}
}

class bar
{
	public $s1;
	public $s2;
	public $s3;

	function __construct()
	{
		global $sjis, $jis, $euc_jp;

		$this->s1 = $euc_jp;
		$this->s2 = $euc_jp;
		$this->s3 = $euc_jp;
	}
}


$o = new foo;
$oo = $o;
$encoding = mb_convert_variables('EUC-JP', 'auto', $oo);
print("$encoding\n");   // EUC-JP
print("{$oo->s1}{$oo->s2}{$oo->s3}\n"); // Converted to EUC-JP

$o = new bar;
$oo = $o;
$encoding = mb_convert_variables('EUC-JP', 'auto', $oo);
print("$encoding\n"); // EUC-JP
print("{$oo->s1}{$oo->s2}{$oo->s3}\n"); // Converted to EUC-JP


// Test for scaler, array and object
echo "== SCALER, ARRAY AND OBJECT TEST ==\n";

$s1 = $euc_jp;
$s2 = $euc_jp;
$s3 = $euc_jp;
$aa = $a;
$oo = $o;

$encoding = mb_convert_variables('EUC-JP', 'auto', $s1, $s2, $s3, $aa, $oo);
print("$encoding\n"); // EUC-JP
print("$s1$s2$s3\n"); // Converted to EUC-JP
print("{$aa[0]}{$aa[1]}{$aa[2]}\n"); // Converted to EUC-JP
print("{$oo->s1}{$oo->s2}{$oo->s3}\n"); // Converted to EUC-JP


?>
--EXPECT--
== SCALER TEST ==
SJIS
日本語テキストです。01234５６７８９。
JIS
日本語テキストです。01234５６７８９。
EUC-JP
k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
EUC-JP
GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== ARRAY TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== OBJECT TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== SCALER, ARRAY AND OBJECT TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
