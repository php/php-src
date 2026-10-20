--TEST--
Reflection::getModifierNames() basic
--FILE--
<?php

function printModifiers($modifiers) {
	echo implode(',', Reflection::getModifierNames($modifiers)), PHP_EOL;
}

printModifiers(ReflectionProperty::IS_PRIVATE);
printModifiers(ReflectionProperty::IS_PROTECTED | ReflectionProperty::IS_STATIC);
printModifiers(ReflectionProperty::IS_PUBLIC | ReflectionProperty::IS_READONLY);
printModifiers(ReflectionClass::IS_EXPLICIT_ABSTRACT);
printModifiers(ReflectionMethod::IS_ABSTRACT | ReflectionMethod::IS_FINAL);
printModifiers(ReflectionProperty::IS_PUBLIC | ReflectionProperty::IS_STATIC | ReflectionProperty::IS_READONLY);
printModifiers(ReflectionClass::IS_READONLY);
printModifiers(ReflectionProperty::IS_VIRTUAL);
printModifiers(ReflectionProperty::IS_PROTECTED_SET);
printModifiers(ReflectionProperty::IS_PRIVATE_SET);
?>
--EXPECT--
private
protected,static
public,readonly
abstract
abstract,final
public,static,readonly
readonly
virtual
protected(set)
private(set)
