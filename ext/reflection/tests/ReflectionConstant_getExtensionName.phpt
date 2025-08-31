--TEST--
ReflectionConstant::getExtensionName()
--EXTENSIONS--
json
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('PHP_VERSION');
var_dump($reflectionConstant->getExtensionName());

$reflectionConstant = new ReflectionConstant('JSON_ERROR_NONE');
var_dump($reflectionConstant->getExtensionName());

const CT_CONST = 5;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getExtensionName());

define('RT_CONST', 6);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->getExtensionName());
?>
--EXPECT--
string(4) "Core"
string(4) "json"
bool(false)
bool(false)
