--TEST--
ReflectionParameter::isDefaultValueConstant() && getDefaultValueConstantName() for namespace
--FILE--
<?php

namespace ReflectionTestNamespace {
    CONST TEST_CONST_1 = "Test Const 1";

    class TestClass {
        const TEST_CONST_2 = "Test Const 2 in class";
    }
}

namespace {
    function ReflectionParameterTest($test=ReflectionTestNamespace\TestClass::TEST_CONST_2, $test2 = ReflectionTestNamespace\CONST_TEST_1) {
        echo $test;
    }
    $reflect = new ReflectionFunction('ReflectionParameterTest');
    foreach($reflect->getParameters() as $param) {
        if($param->isDefaultValueAvailable() && @$param->isDefaultValueConstant()) {
            echo @$param->getDefaultValueConstantName() . "\n";
        }
    }
}
?>
--EXPECT--
ReflectionTestNamespace\TestClass::TEST_CONST_2
ReflectionTestNamespace\CONST_TEST_1
