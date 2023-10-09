--TEST--
mb_convert_variables()
--EXTENSIONS--
mbstring
--INI--
output_handler=
mbstring.language=Japanese
--FILE--
<?php
// TODO: Add more tests
//$debug = true; // Uncomment this line to view error/warning/notice message in *.out file
ini_set('include_path', __DIR__);
include_once('common.inc');

// SJIS string (BASE64 encoded)
$sjis = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
// JIS string (BASE64 encoded)
$jis = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
// EUC-JP string
$euc_jp = mb_convert_encoding("日本語テキストです。01234５６７８９。", 'EUC-JP', 'UTF-8');

// Test for single scalar
echo "== SCALAR TEST ==\n";
$s = $sjis;
$encoding = mb_convert_variables('EUC-JP', 'SJIS', $s);
print("$encoding\n"); // SJIS
echo bin2hex($s), "\n"; // Converted to EUC-JP

$s = $jis;
$encoding = mb_convert_variables('EUC-JP', 'JIS', $s);
print("$encoding\n"); // JIS
echo bin2hex($s), "\n"; // Converted to EUC-JP

$s = $euc_jp;
$encoding = mb_convert_variables('SJIS', 'EUC-JP', $s);
print("$encoding\n"); // EUC-JP
print(base64_encode($s)."\n"); // Converted to SJIS (base64 encoded)

$s = $euc_jp;
$encoding = mb_convert_variables('JIS', 'EUC-JP', $s);
print("$encoding\n"); // EUC-JP
print(base64_encode($s)."\n"); // Converted to JIS (base64 encoded)

// Test for multiple scalar
$s1 = $euc_jp;
$s2 = $euc_jp;
$s3 = $euc_jp;
$encoding = mb_convert_variables('EUC-JP', 'auto', $s1, $s2, $s3);
print("$encoding\n"); // EUC-JP
echo bin2hex("$s1$s2$s3"), "\n"; // Converted to EUC-JP

// Note: Mixing encoding in array/object is not supported
echo "== ARRAY TEST ==\n";
$a = array($s3, $s2, $s1);
$aa = $a;
$encoding = mb_convert_variables('EUC-JP', 'auto', $aa);
print("$encoding\n"); // EUC-JP
echo bin2hex("{$aa[0]}{$aa[1]}{$aa[2]}"), "\n"; // Converted to EUC-JP

$a = array($s1, $s2, $s3);
$aa = $a;
$encoding = mb_convert_variables('EUC-JP', 'auto', $aa);
print("$encoding\n"); // EUC-JP
echo bin2hex("{$aa[0]}{$aa[1]}{$aa[2]}"), "\n"; // Converted to EUC-JP

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
echo bin2hex("{$oo->s1}{$oo->s2}{$oo->s3}"), "\n"; // Converted to EUC-JP

$o = new bar;
$oo = $o;
$encoding = mb_convert_variables('EUC-JP', 'auto', $oo);
print("$encoding\n"); // EUC-JP
echo bin2hex("{$oo->s1}{$oo->s2}{$oo->s3}"), "\n"; // Converted to EUC-JP

// Test for scalar, array and object
echo "== SCALAR, ARRAY AND OBJECT TEST ==\n";

$s1 = $euc_jp;
$s2 = $euc_jp;
$s3 = $euc_jp;
$a = array($s1, $s2, $s3);
$aa = $a;
$oo = $o;

$encoding = mb_convert_variables('EUC-JP', 'auto', $s1, $s2, $s3, $aa, $oo);
print("$encoding\n"); // EUC-JP
echo bin2hex("$s1$s2$s3"), "\n"; // Converted to EUC-JP
echo bin2hex("{$aa[0]}{$aa[1]}{$aa[2]}"), "\n"; // Converted to EUC-JP
echo bin2hex("{$oo->s1}{$oo->s2}{$oo->s3}"), "\n"; // Converted to EUC-JP

echo "== DEEPLY NESTED OBJECT/ARRAY TEST ==\n";

class Nested
{
    public $inner;

    function __construct($value)
    {
        $this->inner = $value;
    }
}

$deeplyNested = array(new Nested(array(new Nested(array(new Nested("BLAH"))))));

$encoding = mb_convert_variables('UTF-16LE', 'UTF-8', $deeplyNested);
echo $encoding, "\n";
echo bin2hex($deeplyNested[0]->inner[0]->inner[0]->inner), "\n";

echo "== INVALID STRING ENCODING TEST ==\n";
// Make sure both that the correct invalid encoding marker is used,
// and that the count of illegal characters is incremented

$illegalCount = mb_get_info('illegal_chars');
$nested = array(new Nested("\xFF"));
mb_substitute_character(0x25);
mb_convert_variables('UTF-16LE', 'UTF-8', $nested);
echo bin2hex($nested[0]->inner), "\n";
echo "# of illegal characters detected: ", mb_get_info('illegal_chars') - $illegalCount, "\n";

$illegalCount = mb_get_info('illegal_chars');
$nested = array(new Nested("\xFF"));
mb_substitute_character(0x26);
mb_convert_variables('UTF-16LE', 'UTF-8', $nested);
echo bin2hex($nested[0]->inner), "\n";
echo "# of illegal characters detected: ", mb_get_info('illegal_chars') - $illegalCount, "\n";

echo "== ENCODING AUTO-DETECTION TEST ==\n";

ini_set('mbstring.strict_detection', '1');
$bad_utf7 = "abc + abc";
var_dump(mb_convert_variables('UTF-8', 'UTF-7,UTF-8', $bad_utf7));
var_dump($bad_utf7);

$bad_utf7imap = "abc &";
var_dump(mb_convert_variables('UTF-8', 'UTF7-IMAP,UTF-8', $bad_utf7imap));
var_dump($bad_utf7imap);

?>
--EXPECT--
== SCALAR TEST ==
SJIS
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
JIS
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
EUC-JP
k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
EUC-JP
GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
== ARRAY TEST ==
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
== OBJECT TEST ==
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
== SCALAR, ARRAY AND OBJECT TEST ==
EUC-JP
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3
== DEEPLY NESTED OBJECT/ARRAY TEST ==
UTF-8
42004c0041004800
== INVALID STRING ENCODING TEST ==
2500
# of illegal characters detected: 1
2600
# of illegal characters detected: 1
== ENCODING AUTO-DETECTION TEST ==
string(5) "UTF-8"
string(9) "abc + abc"
string(5) "UTF-8"
string(5) "abc &"
