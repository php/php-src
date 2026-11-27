--TEST--
Bug GH-7980 (Unexpected result for iconv_mime_decode)
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (@iconv("windows-1258", "UTF-8", "") === false) {
    die("skip windows-1258 is not supported by this iconv implementation");
}
?>
--FILE--
<?php
$subject = '=?windows-1258?Q?DSI_Charg=E9_de_Formation_Jean_Dupont?= <jdupont@example.fr>';
var_dump(iconv_mime_decode($subject, ICONV_MIME_DECODE_STRICT, 'UTF-8'));
?>
--EXPECT--
string(57) "DSI Chargé de Formation Jean Dupont <jdupont@example.fr>"
