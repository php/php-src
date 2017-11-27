--TEST--
mb_strcut() - unknown encoding
-- CREDITS --
Jachim Coudenys <jachimcoudenys@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
mb_strcut('coudenys', 0, 4, 'jachim');
?>
--EXPECTF--
Warning: mb_strcut(): Unknown encoding "jachim" in %s on line %d
