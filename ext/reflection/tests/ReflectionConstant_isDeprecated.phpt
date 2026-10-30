--TEST--
ReflectionConstant::isDeprecated()
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('ZEND_CONSTANT_A');
var_dump($reflectionConstant->isDeprecated());

$reflectionConstant = new ReflectionConstant('ZEND_TEST_DEPRECATED');
var_dump($reflectionConstant->isDeprecated());

define('RT_CONST', 42);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->isDeprecated());

const CT_CONST = 43;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->isDeprecated());

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)
