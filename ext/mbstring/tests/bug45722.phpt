--TEST--
Bug #45722 (mb_check_encoding() crashes)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_check_encoding("&\xc2\xb7 TEST TEST TEST TEST TEST TEST", "HTML-ENTITIES"));
?>
--EXPECT--
bool(false)
