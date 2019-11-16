<?php

class ReflectionException extends Exception
{
}

class Reflection
{
    /** @return string[] */
    public static function getModifierNames(int $modifiers) {}

    public static function export(Reflector $reflector, bool $return = false) {}
}

interface Reflector
{
    /** @return string */
    public function __toString();
}

abstract class ReflectionFunctionAbstract implements Reflector
{
    final private function __clone() {}

    /** @return bool */
    public function inNamespace() {}

    /** @return bool */
    public function isClosure() {}

    /** @return bool */
    public function isDeprecated() {}

    /** @return bool */
    public function isInternal() {}

    /** @return bool */
    public function isUserDefined() {}

    /** @return bool */
    public function isGenerator() {}

    /** @return bool */
    public function isVariadic() {}

    /** @return ?object */
    public function getClosureThis() {}

    /** @return ?ReflectionClass */
    public function getClosureScopeClass() {}

    /** @return string|false */
    public function getDocComment() {}

    /** @return int|false */
    public function getEndLine() {}

    /** @return ?ReflectionExtension */
    public function getExtension() {}

    /** @return string|false */
    public function getExtensionName() {}

    /** @return string|false */
    public function getFileName() {}

    /** @return string|false */
    public function getName() {}

    /** @return string|false */
    public function getNamespaceName() {}

    /** @return int */
    public function getNumberOfParameters() {}

    /** @return int */
    public function getNumberOfRequiredParameters() {}

    /** @return ReflectionParameter[] */
    public function getParameters() {}

    /** @return string|false */
    public function getShortName() {}

    /** @return int|false */
    public function getStartLine() {}

    /** @return array */
    public function getStaticVariables() {}

    /** @return bool */
    public function returnsReference() {}

    /** @return bool */
    public function hasReturnType() {}

    /** @return ?ReflectionType */
    public function getReturnType() {}
}

class ReflectionFunction extends ReflectionFunctionAbstract
{
    /** @param string|Closure $name */
    public function __construct($name) {}

    /** @return string */
    public function __toString() {}

    public static function export($name, bool $return = false) {}

    /** @return bool */
    public function isDisabled() {}

    public function invoke(...$args) {}

    public function invokeArgs(array $args) {}

    /** @return Closure */
    public function getClosure() {}
}

class ReflectionGenerator
{
    public function __construct(Generator $generator) {}

    /** @return int */
    public function getExecutingLine() {}

    /** @return string */
    public function getExecutingFile() {}

    /** @return array */
    public function getTrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT) {}

    /** @return ReflectionFunctionAbstract */
    public function getFunction() {}

    /** @return ?object */
    public function getThis() {}

    /** @return Generator */
    public function getExecutingGenerator() {}
}

class ReflectionMethod extends ReflectionFunctionAbstract
{
    /** @param object|string $class_or_method */
    public function __construct($class_or_method, string $name = UNKNOWN) {}

    /** @return string */
    public function __toString() {}

    public static function export($class, $name, bool $return = false) {}

    /** @return bool */
    public function isPublic() {}

    /** @return bool */
    public function isPrivate() {}

    /** @return bool */
    public function isProtected() {}

    /** @return bool */
    public function isAbstract() {}

    /** @return bool */
    public function isFinal() {}

    /** @return bool */
    public function isStatic() {}

    /** @return bool */
    public function isConstructor() {}

    /** @return bool */
    public function isDestructor() {}

    /** @return Closure */
    public function getClosure($object = UNKNOWN) {}

    /** @return int */
    public function getModifiers() {}

    public function invoke(?object $object = null, ...$args) {}

    public function invokeArgs(?object $object, array $args) {}

    /** @return ReflectionClass */
    public function getDeclaringClass() {}

    /** @return ReflectionMethod */
    public function getPrototype() {}

    /** @return void */
    public function setAccessible(bool $visible) {}
}

class ReflectionClass implements Reflector
{
    final private function __clone() {}

    public static function export($argument, bool $return = false) {}

    /** @param object|string $argument */
    public function __construct($argument) {}

    /** @return string */
    public function __toString() {}

    /** @return string|false */
    public function getName() {}

    /** @return bool */
    public function isInternal() {}

    /** @return bool */
    public function isUserDefined() {}

    /** @return bool */
    public function isAnonymous() {}

    /** @return bool */
    public function isInstantiable() {}

    /** @return bool */
    public function isCloneable() {}

    /** @return string|false */
    public function getFileName() {}

    /** @return int|false */
    public function getStartLine() {}

    /** @return int|false */
    public function getEndLine() {}

    /** @return string|false */
    public function getDocComment() {}

    /** @return ?ReflectionMethod */
    public function getConstructor() {}

    /** @return bool */
    public function hasMethod(string $name) {}

    /** @return ReflectionMethod */
    public function getMethod(string $name) {}

    /** @return ReflectionMethod[] */
    public function getMethods(?int $filter = null) {}

    /** @return bool */
    public function hasProperty(string $name) {}

    /** @return ReflectionProperty */
    public function getProperty(string $name) {}

    /** @return ReflectionProperty[] */
    public function getProperties(?int $filter = null) {}

    /** @return bool */
    public function hasConstant(string $name) {}

    /** @return ?array */
    public function getConstants() {}

    /** @return ReflectionClassConstant[] */
    public function getReflectionConstants() {}

    public function getConstant(string $name) {}

    /** @return ReflectionClassConstant|false */
    public function getReflectionConstant(string $name) {}

    /** @return ReflectionClass[] */
    public function getInterfaces() {}

    /** @return string[] */
    public function getInterfaceNames() {}

    /** @return bool */
    public function isInterface() {}

    /** @return ReflectionClass[] */
    public function getTraits() {}

    /** @return string[] */
    public function getTraitNames() {}

    /** @return string[] */
    public function getTraitAliases() {}

    /** @return bool */
    public function isTrait() {}

    /** @return bool */
    public function isAbstract() {}

    /** @return bool */
    public function isFinal() {}

    /** @return int */
    public function getModifiers() {}

    /** @return bool */
    public function isIntance(object $object) {}

    /** @return object */
    public function newInstance(...$args) {}

    /** @return object */
    public function newInstanceWithoutConstructor() {}

    /** @return object */
    public function newInstanceArgs(array $args = []) {}

    /** @return ReflectionClass|false */
    public function getParentClass() {}

    /**
     * @param string|ReflectionClass $class
     * @return bool
     */
    public function isSubclassOf($class) {}

    /** @return ?array */
    public function getStaticProperties() {}

    public function getStaticPropertyValue(string $name, $default = UNKNOWN) {}

    /** @return void */
    public function setStaticPropertyValue(string $name, $value) {}

    /** @return array */
    public function getDefaultProperties() {}

    /** @return bool */
    public function isIterable() {}

    /** @return bool */
    public function isIterateable() {}

    /**
     * @param string|ReflectionClass $interface
     * @return bool
     */
    public function implementsInterface($interface) {}

    /** @return ?ReflectionExtension */
    public function getExtension() {}

    /** @return string|false */
    public function getExtensionName() {}

    /** @return bool */
    public function inNamespace() {}

    /** @return string|false */
    public function getNamespaceName() {}

    /** @return string|false */
    public function getShortName() {}
}

class ReflectionObject extends ReflectionClass
{
    public function __construct(object $argument) {}

    public static function export($argument, bool $return = false) {}
}

class ReflectionProperty implements Reflector
{
    final private function __clone() {}

    public static function export($class, $name, bool $return = false) {}

    /** @param string|object $class */
    public function __construct($class, string $name) {}

    /** @return string */
    public function __toString() {}

    /** @return string|false */
    public function getName() {}

    public function getValue(?object $object = null) {}

    /** @return void */
    public function setValue($object_or_value, $value = UNKNOWN) {}

    /** @return bool */
    public function isInitialized(object $object = UNKNOWN) {}

    /** @return bool */
    public function isPublic() {}

    /** @return bool */
    public function isPrivate() {}

    /** @return bool */
    public function isProtected() {}

    /** @return bool */
    public function isStatic() {}

    /** @return bool */
    public function isDefault() {}

    /** @return int */
    public function getModifiers() {}

    /** @return ReflectionClass */
    public function getDeclaringClass() {}

    /** @return string|false */
    public function getDocComment() {}

    /** @return void */
    public function setAccessible(bool $visible) {}

    /** @return ?ReflectionType */
    public function getType() {}

    /** @return bool */
    public function hasType() {}
}

class ReflectionClassConstant implements Reflector
{
    final private function __clone() {}

    public static function export($class, $name, bool $return = false) {}

    /** @return string|object */
    public function __construct($class, string $name) {}

    /** @return string */
    public function __toString() {}

    /** @return string|false */
    public function getName() {}

    public function getValue() {}

    /** @return bool */
    public function isPublic() {}

    /** @return bool */
    public function isPrivate() {}

    /** @return bool */
    public function isProtected() {}

    /** @return int */
    public function getModifiers() {}

    /** @return ReflectionClass */
    public function getDeclaringClass() {}

    /** @return string|false */
    public function getDocComment() {}
}

class ReflectionParameter implements Reflector
{
    final private function __clone() {}

    public static function export($function, $parameter, bool $return = false) {}

    /**
     * @param string|array|object
     * @param int|string
     */
    public function __construct($function,  $parameter) {}

    /** @return string */
    public function __toString() {}

    /** @return string|false */
    public function getName() {}

    /** @return bool */
    public function isPassedByReference() {}

    /** @return bool */
    public function canBePassedByValue() {}

    /** @return ReflectionFunctionAbstract */
    public function getDeclaringFunction() {}

    /** @return ?ReflectionClass */
    public function getDeclaringClass() {}

    /** @return ?ReflectionClass */
    public function getClass() {}

    /** @return bool */
    public function hasType() {}

    /** @return ?ReflectionType */
    public function getType() {}

    /** @return bool */
    public function isArray() {}

    /** @return bool */
    public function isCallable() {}

    /** @return bool */
    public function allowsNull() {}

    /** @return int */
    public function getPosition() {}

    /** @return bool */
    public function isOptional() {}

    /** @return bool */
    public function isDefaultValueAvailable() {}

    public function getDefaultValue() {}

    /** @return bool */
    public function isDefaultValueConstant() {}

    /** @return ?string */
    public function getDefaultValueConstantName() {}

    /** @return bool */
    public function isVariadic() {}
}

abstract class ReflectionType
{
    final private function __clone() {}

    /** @return bool */
    public function allowsNull() {}

    /** @return string */
    public function __toString() {}
}

class ReflectionNamedType extends ReflectionType
{
    /** @return string */
    public function getName() {}

    /** @return bool */
    public function isBuiltin() {}
}

class ReflectionUnionType extends ReflectionType
{
    public function getTypes(): array {}
}

class ReflectionExtension implements Reflector
{
    final private function __clone() {}

    public static function export($name, bool $return = false) {}

    public function __construct(string $name) {}

    /** @return string */
    public function __toString() {}

    /** @return string|false */
    public function getName() {}

    /** @return ?string */
    public function getVersion() {}

    /** @return ReflectionFunction[] */
    public function getFunctions() {}

    /** @return array */
    public function getConstants() {}

    /** @return array */
    public function getINIEntries() {}

    /** @return ReflectionClass[] */
    public function getClasses() {}

    /** @return string[] */
    public function getClassNames() {}

    /** @return array */
    public function getDependencies() {}

    /** @return void */
    public function info() {}

    /** @return bool */
    public function isPersistent() {}

    /** @return bool */
    public function isTemporary() {}
}

class ReflectionZendExtension implements Reflector
{
    final private function __clone() {}

    public static function export($name, bool $return = false) {}

    public function __construct(string $name) {}

    /** @return string */
    public function __toString() {}

    /** @return string */
    public function getName() {}

    /** @return string */
    public function getVersion() {}

    /** @return string */
    public function getAuthor() {}

    /** @return string */
    public function getURL() {}

    /** @return string */
    public function getCopyright() {}
}

final class ReflectionReference
{
    /** @param int|string $key */
    public static function fromArrayElement(array $array, $key): ?ReflectionReference {}

    public function getId(): string {}

    private function __clone() {}

    private function __construct() {}
}
