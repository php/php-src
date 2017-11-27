--TEST--
mb_strwidth() - unknown encoding
--CREDITS--
Jachim Coudenys <jachimcoudenys@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
mb_strwidth('coudenys', 'jachim');
?>
--EXPECTF--
Warning: mb_strwidth(): Unknown encoding "jachim" in %s on line %d
