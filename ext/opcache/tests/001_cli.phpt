--TEST--
001: O+ works in CLI
--INI--
zend_optimizerplus.enable=1
zend_optimizerplus.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$config = accelerator_get_configuration();
$status = accelerator_get_status();
var_dump($config["directives"]["zend_optimizerplus.enable"]);
var_dump($config["directives"]["zend_optimizerplus.enable_cli"]);
var_dump($status["accelerator_enabled"]);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
