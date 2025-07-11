--TEST--
GH-18534 (FPM exit code 70 with enabled opcache and hooked properties in traits)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_OS_FAMILY === 'Windows') die('skip preloading does not work on Windows'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/gh18534_preload.inc
--FILE--
<?php

require_once __DIR__ . '/gh18534_preload.inc';

$test = new DummyModel;
var_dump($test->dummyProperty2);

?>
--EXPECT--
NULL
