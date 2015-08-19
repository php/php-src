--TEST--
Bug #70290 (Null pointer deref (segfault) in spl_autoload via ob_start)
--INI--
display_errors=2
--FILE--
<?php ob_start("spl_autoload"); ?> 1
--EXPECT--
Fatal error: Unknown: Class  1
 could not be loaded in Unknown on line 0
