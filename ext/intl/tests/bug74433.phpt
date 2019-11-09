--TEST--
Bug #74433 Wrong reflection on the Normalizer methods
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not available'); ?>
--FILE--
<?php
$rm = new ReflectionMethod(Normalizer::class, 'isNormalized');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
$rm = new ReflectionMethod(Normalizer::class, 'normalize');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
?>
--EXPECT--
int(2)
int(1)
int(2)
int(1)
