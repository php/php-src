--TEST--
Bug #69838 OPCACHE Warning Internal error: wrong size calculation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.log_verbosity_level=2
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$x = gethostbyname("localhost");

?>
===DONE===
--EXPECT--
===DONE===
