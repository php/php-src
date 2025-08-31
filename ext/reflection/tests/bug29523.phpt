--TEST--
Reflection Bug #29523 (ReflectionParameter::isOptional() is incorrect)
--FILE--
<?php

class TestClass
{
}

function optionalTest(TestClass $a, TestClass $b, $c = 3)
{
}

$function = new ReflectionFunction('optionalTest');
$numberOfNotOptionalParameters = 0;
$numberOfOptionalParameters = 0;
foreach($function->getParameters() as $parameter)
{
    var_dump($parameter->isOptional());
    if ($parameter->isOptional())
    {
        ++$numberOfOptionalParameters;
    }
    else
    {
        ++$numberOfNotOptionalParameters;
    }
}
var_dump($function->getNumberOfRequiredParameters());
var_dump($numberOfNotOptionalParameters);

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
int(2)
int(2)
