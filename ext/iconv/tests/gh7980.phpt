--TEST--
Bug GH-7980 (Unexpected result for iconv_mime_decode)
--SKIPIF--
<?php
if (!extension_loaded("iconv")) die("skip iconv extension not available");
?>
--FILE--
<?php
$subject = '=?windows-1258?Q?DSI_Charg=E9_de_Formation_Jean_Dupont?= <jdupont@example.fr>';
var_dump(iconv_mime_decode($subject, ICONV_MIME_DECODE_STRICT, 'UTF-8'));
?>
--EXPECT--
string(57) "DSI Chargé de Formation Jean Dupont <jdupont@example.fr>"
