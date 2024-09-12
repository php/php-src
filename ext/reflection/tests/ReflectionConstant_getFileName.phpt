--TEST--
ReflectionConstant::getFileName()
--FILE--
<?php

include "included5.inc";

$reflectionConstant = new ReflectionConstant('PHP_VERSION');
var_dump($reflectionConstant->getFileName());

define('IN_CURRENT_FILE', 42);
$reflectionConstant = new ReflectionConstant('IN_CURRENT_FILE');
var_dump($reflectionConstant->getFileName());

$reflectionConstant = new ReflectionConstant('INCLUDED_CONSTANT_DEFINED');
var_dump($reflectionConstant->getFileName());

$reflectionConstant = new ReflectionConstant('INCLUDED_CONSTANT_AST');
var_dump($reflectionConstant->getFileName());

?>
--EXPECTF--
bool(false)
string(%d) "%sReflectionConstant_getFileName.php"
string(%d) "%sincluded5.inc"
string(%d) "%sincluded5.inc"
