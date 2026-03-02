--TEST--
ReflectionConstant::getAttributes() when there are none
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('E_ERROR');
var_dump($reflectionConstant->getAttributes());

define('RT_CONST', 42);
$reflectionConstant = new ReflectionConstant('RT_CONST');
var_dump($reflectionConstant->getAttributes());

const CT_CONST = 43;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getAttributes());

?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
