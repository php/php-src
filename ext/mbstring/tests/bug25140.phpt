--TEST--
Bug #25140 (mb_convert_encoding returns FALSE on an empty string)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump( mb_convert_encoding( '', 'SJIS', 'EUC-JP' ) );
?>
--EXPECT--
string(0) ""
