--TEST--
mb_encoding_aliases()
--CREDITS--
PHPTestFest 2017 
Bertrand Drouhard <bertrand@drouhard.com>
phpdublin
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_encoding_aliases("THIS_ENCODING_DOES_NOT_EXIST");
?>
--EXPECTF--
Warning: mb_encoding_aliases(): Unknown encoding "THIS_ENCODING_DOES_NOT_EXIST" in %s on line %d