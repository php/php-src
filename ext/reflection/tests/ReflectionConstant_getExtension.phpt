--TEST--
ReflectionConstant::getExtension()
--EXTENSIONS--
json
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('PHP_VERSION');
var_dump($reflectionConstant->getExtension());

$reflectionConstant = new ReflectionConstant('JSON_ERROR_NONE');
var_dump($reflectionConstant->getExtension());

const CT_CONST = 5;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getExtension());

define('RT_CONST', 6);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->getExtension());
?>
--EXPECTF--
object(ReflectionExtension)#%d (1) {
  ["name"]=>
  string(4) "Core"
}
object(ReflectionExtension)#%d (1) {
  ["name"]=>
  string(4) "json"
}
NULL
NULL
