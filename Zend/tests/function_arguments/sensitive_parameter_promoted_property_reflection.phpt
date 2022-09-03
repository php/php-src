--TEST--
The SensitiveParameter attribute can not be instantiated against property
--FILE--
<?php

class WithSensitiveParameter
{
    public function __construct(#[SensitiveParameter] public string $password)
    {
    }
}

// ok
$parameterAttribute = (new ReflectionClass(WithSensitiveParameter::class))->getConstructor()->getParameters()[0]->getAttributes()[0]->newInstance();
// NOT ok
$propertyAttribute = (new ReflectionClass(WithSensitiveParameter::class))->getProperties()[0]->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Fatal error: Uncaught Error: Attribute "SensitiveParameter" cannot target property (allowed targets: parameter) in %ssensitive_parameter_promoted_property_reflection.php:13
Stack trace:
#0 %ssensitive_parameter_promoted_property_reflection.php(13): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %ssensitive_parameter_promoted_property_reflection.php on line 13
