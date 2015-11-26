--TEST--
Bug #25140 (mb_convert_encoding returns FALSE on an empty string)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump( mb_convert_encoding( '', 'SJIS', 'EUC-JP' ) );
?>
--EXPECT--
string(0) ""
