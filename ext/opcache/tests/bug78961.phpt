--TEST--
Bug #78961 (erroneous optimization of re-assigned $GLOBALS)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$GLOBALS = array();
$GLOBALS['td'] = array();
$GLOBALS['td']['nsno'] = 3;
var_dump($GLOBALS['td']['nsno']);
?>
--EXPECT--
int(3)
