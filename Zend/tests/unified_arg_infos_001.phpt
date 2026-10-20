--TEST--
Declaring non persistent method with arg info
--EXTENSIONS--
zend_test
--FILE--
<?php

$o = new _ZendTestClass();
$o->testTmpMethodWithArgInfo(null);

echo new ReflectionFunction($o->testTmpMethodWithArgInfo(...));

?>
--EXPECT--
Closure [ <internal> public method testTmpMethodWithArgInfo ] {

  - Parameters [2] {
    Parameter #0 [ <optional> Foo|Bar|null $tmpMethodParamName = null ]
    Parameter #1 [ <optional> string $tmpMethodParamWithStringDefaultValue = "tmpMethodParamWithStringDefaultValue" ]
  }
}
