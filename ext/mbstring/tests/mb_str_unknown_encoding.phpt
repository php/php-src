--TEST--
mb_str*() - unknown encoding
--CREDITS--
Jachim Coudenys <jachimcoudenys@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_chr(1, 'UTF-0');
mb_convert_case('coudenys', MB_CASE_UPPER, 'UTF-0');
mb_convert_encoding('coudenys', 'UTF-8', 'UTF-0');
mb_convert_kana('coudenys', 'KV', 'UTF-0');
mb_decode_numericentity('coudenys', 'KV', 'UTF-0');
mb_ord('coudenys', 'UTF-0');
mb_strcut('coudenys', 0, 4, 'UTF-0');
mb_strimwidth('coudenys', 0, 4, '', 'UTF-0');
mb_stripos('coudenys', 'cou', 0, 'UTF-0');
mb_stristr('coudenys', 'cou', false, 'UTF-0');
mb_strlen('coudenys', 'UTF-0');
mb_strpos('coudenys', 'cou', 0, 'UTF-0');
mb_strrchr('coudenys', 'cou', false, 'UTF-0');
mb_strrichr('coudenys', 'cou', false, 'UTF-0');
mb_strripos('coudenys', 'cou', 0, 'UTF-0');
mb_strrpos('coudenys', 'cou', 0, 'UTF-0');
mb_strstr('coudenys', 'cou', false, 'UTF-0');
mb_strtolower('coudenys', 'UTF-0');
mb_strtoupper('coudenys', 'UTF-0');
mb_strwidth('coudenys', 'UTF-0');
mb_substr('coudenys', 0, null, 'UTF-0');
mb_substr_count('coudenys', 'c', 'UTF-0');
?>
--EXPECTF--
Warning: mb_chr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_convert_case(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_convert_encoding(): Illegal character encoding specified in %s on line %d

Warning: mb_convert_kana(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_decode_numericentity(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_ord(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strcut(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strimwidth(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_stripos(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_stristr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strlen(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strpos(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strrchr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strrichr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strripos(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strrpos(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strstr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strtolower(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strtoupper(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_strwidth(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_substr(): Unknown encoding "UTF-0" in %s on line %d

Warning: mb_substr_count(): Unknown encoding "UTF-0" in %s on line %d

