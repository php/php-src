--TEST--
ReflectionConstant::__toString()
--FILE--
<?php

const IS_TRUE = true;
const IS_FALSE = false;

echo new ReflectionConstant('IS_TRUE');
echo new ReflectionConstant('IS_FALSE');

?>
--EXPECT--
Constant [ bool IS_TRUE ] { true }
Constant [ bool IS_FALSE ] { false }
