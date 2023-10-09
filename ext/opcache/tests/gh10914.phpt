--TEST--
GH-10914 (OPCache with Enum and Callback functions results in segmentation fault)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
opcache.preload={PWD}/preload_gh10914.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$x = new ReflectionEnum(ExampleEnum::class);
var_dump($x->getCases()[0]->getValue());
var_dump($x->getCases()[0]->getBackingValue());
var_dump($x->getCase('FIRST')->getValue());
var_dump($x->getCase('FIRST')->getBackingValue());
?>
--EXPECT--
enum(ExampleEnum::FIRST)
string(4) "AAAb"
enum(ExampleEnum::FIRST)
string(4) "AAAb"
