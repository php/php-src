--TEST--
Test that the directives listed with `opcache_get_configuration` include all those from the ini settings.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$opts = opcache_get_configuration()['directives'];
$inis = ini_get_all('zend opcache');
var_dump(array_diff_key($inis, $opts));
?>
--EXPECT--
array(0) {
}