--TEST--
Bug #69079 (enhancement for mb_substitute_character)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_internal_encoding('UTF-8');
var_dump(mb_substitute_character(0x1f600));
mb_internal_encoding('EUC-JP-2004');
var_dump(mb_substitute_character(0x8fa1ef));
?>
--EXPECT--
bool(true)
bool(true)