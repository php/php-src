--TEST--
Bug #65683: Intl does not support DateTimeImmutable (using datefmt_format_object)
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
--FILE--
<?php
$date = date_create('1970-01-01');
$date_imm = date_create_immutable('1970-01-01');
var_dump(datefmt_format_object($date, null, 'fr_FR'));
var_dump(datefmt_format_object($date_imm, null, 'fr_FR'));
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
