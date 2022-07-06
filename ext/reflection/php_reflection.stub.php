<?php

/** @generate-function-entries */

class ReflectionException extends Exception
{
}

class Reflection
{
    /** @return string[] */
    public static function getModifierNames(int $modifiers) {}
}

interface Reflector extends Stringable
{
}

abstract class ReflectionFunctionAbstract implements Reflector
{
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

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

    /** @return object|null */
    public function getClosureThis() {}

    /** @return ReflectionClass|null */
    public function getClosureScopeClass() {}

    /** @return ReflectionClass|null */
    public function getClosureCalledClass() {}

    /** @return string|false */
    public function getDocComment() {}

    /** @return int|false */
    public function getEndLine() {}

    /** @return ReflectionExtension|null */
    public function getExtension() {}

    /** @return string|false */
    public function getExtensionName() {}

    /** @return string|false */
    public function getFileName() {}

    /** @return string */
    public function getName() {}

    /** @return string */
    public function getNamespaceName() {}

    /** @return int */
    public function getNumberOfParameters() {}

    /** @return int */
    public function getNumberOfRequiredParameters() {}

    /** @return ReflectionParameter[] */
    public function getParameters() {}

    /** @return string */
    public function getShortName() {}

    /** @return int|false */
    public function getStartLine() {}

    /** @return array */
    public function getStaticVariables() {}

    /** @return bool */
    public function returnsReference() {}

    /** @return bool */
    public function hasReturnType() {}

    /** @return ReflectionType|null */
    public function getReturnType() {}

    /** @return ReflectionAttribute[] */
    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionFunction extends ReflectionFunctionAbstract
{
    public function __construct(Closure|string $function) {}

    public function __toString(): string {}

    /**
     * @return bool
     * @deprecated ReflectionFunction can no longer be constructed for disabled functions
     */
    public function isDisabled() {}

    /** @return mixed */
    public function invoke(mixed ...$args) {}

    /** @return mixed */
    public function invokeArgs(array $args) {}

    /** @return Closure */
    public function getClosure() {}
}

final class ReflectionGenerator
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

    /** @return object|null */
    public function getThis() {}

    /** @return Generator */
    public function getExecutingGenerator() {}
}

class ReflectionMethod extends ReflectionFunctionAbstract
{
    public function __construct(object|string $objectOrMethod, ?string $method = null) {}

    public function __toString(): string {}

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
    public function getClosure(?object $object = null) {}

    /** @return int */
    public function getModifiers() {}

    /** @return mixed */
    public function invoke(?object $object, mixed ...$args) {}

    /** @return mixed */
    public function invokeArgs(?object $object, array $args) {}

    /** @return ReflectionClass */
    public function getDeclaringClass() {}

    /** @return ReflectionMethod */
    public function getPrototype() {}

    /** @return void */
    public function setAccessible(bool $accessible) {}
}

class ReflectionClass implements Reflector
{
    final private function __clone(): void {}

    public function __construct(object|string $objectOrClass) {}

    public function __toString(): string {}

    /** @return string */
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

    /** @return ReflectionMethod|null */
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

    /** @return array */
    public function getConstants(?int $filter = null) {}

    /** @return ReflectionClassConstant[] */
    public function getReflectionConstants(?int $filter = null) {}

    /** @return mixed */
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
    public function isInstance(object $object) {}

    /** @return object */
    public function newInstance(mixed ...$args) {}

    /** @return object */
    public function newInstanceWithoutConstructor() {}

    /** @return object */
    public function newInstanceArgs(array $args = []) {}

    /** @return ReflectionClass|false */
    public function getParentClass() {}

    /** @return bool */
    public function isSubclassOf(ReflectionClass|string $class) {}

    /** @return array|null */
    public function getStaticProperties() {}

    /** @return mixed */
    public function getStaticPropertyValue(string $name, mixed $default = UNKNOWN) {}

    /** @return void */
    public function setStaticPropertyValue(string $name, mixed $value) {}

    /** @return array */
    public function getDefaultProperties() {}

    /** @return bool */
    public function isIterable() {}

    /**
     * @return bool
     * @alias ReflectionClass::isIterable
     */
    public function isIterateable() {}

    /** @return bool */
    public function implementsInterface(ReflectionClass|string $interface) {}

    /** @return ReflectionExtension|null */
    public function getExtension() {}

    /** @return string|false */
    public function getExtensionName() {}

    /** @return bool */
    public function inNamespace() {}

    /** @return string */
    public function getNamespaceName() {}

    /** @return string */
    public function getShortName() {}

    /** @return ReflectionAttribute[] */
    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionObject extends ReflectionClass
{
    public function __construct(object $object) {}
}

class ReflectionProperty implements Reflector
{
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    public function __construct(object|string $class, string $property) {}

    public function __toString(): string {}

    /** @return string */
    public function getName() {}

    /** @return mixed */
    public function getValue(?object $object = null) {}

    /** @return void */
    public function setValue(mixed $objectOrValue, mixed $value = UNKNOWN) {}

    /** @return bool */
    public function isInitialized(?object $object = null) {}

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

    public function isPromoted(): bool {}

    /** @return int */
    public function getModifiers() {}

    /** @return ReflectionClass */
    public function getDeclaringClass() {}

    /** @return string|false */
    public function getDocComment() {}

    /** @return void */
    public function setAccessible(bool $accessible) {}

    /** @return ReflectionType|null */
    public function getType() {}

    /** @return bool */
    public function hasType() {}

    public function hasDefaultValue(): bool {}

    /** @return mixed */
    public function getDefaultValue() {}

    /** @return ReflectionAttribute[] */
    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionClassConstant implements Reflector
{
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    public function __construct(object|string $class, string $constant) {}

    public function __toString(): string {}

    /** @return string|false */
    public function getName() {}

    /** @return mixed */
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

    /** @return ReflectionAttribute[] */
    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionParameter implements Reflector
{
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    /** @param string|array|object $function */
    public function __construct($function, int|string $param) {}

    public function __toString(): string {}

    /** @return string */
    public function getName() {}

    /** @return bool */
    public function isPassedByReference() {}

    /** @return bool */
    public function canBePassedByValue() {}

    /** @return ReflectionFunctionAbstract */
    public function getDeclaringFunction() {}

    /** @return ReflectionClass|null */
    public function getDeclaringClass() {}

    /**
     * @return ReflectionClass|null
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function getClass() {}

    /** @return bool */
    public function hasType() {}

    /** @return ReflectionType|null */
    public function getType() {}

    /**
     * @return bool
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function isArray() {}

    /**
     * @return bool
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function isCallable() {}

    /** @return bool */
    public function allowsNull() {}

    /** @return int */
    public function getPosition() {}

    /** @return bool */
    public function isOptional() {}

    /** @return bool */
    public function isDefaultValueAvailable() {}

    /** @return mixed */
    public function getDefaultValue() {}

    /** @return bool */
    public function isDefaultValueConstant() {}

    /** @return string|null */
    public function getDefaultValueConstantName() {}

    /** @return bool */
    public function isVariadic() {}

    public function isPromoted(): bool {}

    /** @return ReflectionAttribute[] */
    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

abstract class ReflectionType implements Stringable
{
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    /** @return bool */
    public function allowsNull() {}

    public function __toString(): string {}
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
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    public function __construct(string $name) {}

    public function __toString(): string {}

    /** @return string */
    public function getName() {}

    /** @return string|null */
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
    /** @implementation-alias ReflectionClass::__clone */
    final private function __clone(): void {}

    public function __construct(string $name) {}

    public function __toString(): string {}

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
    public static function fromArrayElement(array $array, int|string $key): ?ReflectionReference {}

    public function getId(): string {}

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    private function __construct() {}
}

class ReflectionAttribute
{
    public function getName(): string {}
    public function getTarget(): int {}
    public function isRepeated(): bool {}
    public function getArguments(): array {}
    public function newInstance(): object {}

    private function __clone(): void {}

    private function __construct() {}
}
