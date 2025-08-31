--TEST--
ReflectionConstant::getValue()
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('ZEND_CONSTANT_A');
var_dump($reflectionConstant->getValue());

$reflectionConstant = new ReflectionConstant('ZEND_TEST_DEPRECATED');
var_dump($reflectionConstant->getValue());

define('RT_CONST', 42);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->getValue());

define('CT_CONST', 43);
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getValue());

?>
--EXPECT--
string(6) "global"
int(42)
int(42)
int(43)
