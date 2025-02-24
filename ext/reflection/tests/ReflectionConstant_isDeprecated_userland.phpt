--TEST--
ReflectionConstant::isDeprecated() from attribute
--FILE--
<?php

#[Deprecated]
const CT_CONST = 42;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->isDeprecated());

?>
--EXPECT--
bool(true)
