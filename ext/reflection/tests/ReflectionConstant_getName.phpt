--TEST--
ReflectionConstant::getName()
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('ZEND_CONSTANT_A');
var_dump($reflectionConstant->getName());

$reflectionConstant = new ReflectionConstant('ZEND_TEST_DEPRECATED');
var_dump($reflectionConstant->getName());

define('RT_CONST', 42);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->getName());

define('CT_CONST', 43);
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getName());

?>
--EXPECT--
string(15) "ZEND_CONSTANT_A"
string(20) "ZEND_TEST_DEPRECATED"
string(8) "RT_CONST"
string(8) "CT_CONST"
