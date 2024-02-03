<?php

/** @generate-class-entries */

class ReflectionException extends Exception
{
}

class Reflection
{
    /** @tentative-return-type */
    public static function getModifierNames(int $modifiers): array {}
}

interface Reflector extends Stringable
{
}

/** @not-serializable */
abstract class ReflectionFunctionAbstract implements Reflector
{
    public string $name;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    /** @tentative-return-type */
    public function inNamespace(): bool {}

    /** @tentative-return-type */
    public function isClosure(): bool {}

    /** @tentative-return-type */
    public function isDeprecated(): bool {}

    /** @tentative-return-type */
    public function isInternal(): bool {}

    /** @tentative-return-type */
    public function isUserDefined(): bool {}

    /** @tentative-return-type */
    public function isGenerator(): bool {}

    /** @tentative-return-type */
    public function isVariadic(): bool {}

    /** @tentative-return-type */
    public function isStatic(): bool {}

    /** @tentative-return-type */
    public function getClosureThis(): ?object {}

    /** @tentative-return-type */
    public function getClosureScopeClass(): ?ReflectionClass {}

    /** @tentative-return-type */
    public function getClosureCalledClass(): ?ReflectionClass {}

    public function getClosureUsedVariables(): array {}

    /** @tentative-return-type */
    public function getDocComment(): string|false {}

    /** @tentative-return-type */
    public function getEndLine(): int|false {}

    /** @tentative-return-type */
    public function getExtension(): ?ReflectionExtension {}

    /** @tentative-return-type */
    public function getExtensionName(): string|false {}

    /** @tentative-return-type */
    public function getFileName(): string|false {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function getNamespaceName(): string {}

    /** @tentative-return-type */
    public function getNumberOfParameters(): int {}

    /** @tentative-return-type */
    public function getNumberOfRequiredParameters(): int {}

    /** @tentative-return-type */
    public function getParameters(): array {}

    /** @tentative-return-type */
    public function getShortName(): string {}

    /** @tentative-return-type */
    public function getStartLine(): int|false {}

    /** @tentative-return-type */
    public function getStaticVariables(): array {}

    /** @tentative-return-type */
    public function returnsReference(): bool {}

    /** @tentative-return-type */
    public function hasReturnType(): bool {}

    /** @tentative-return-type */
    public function getReturnType(): ?ReflectionType {}

    public function hasTentativeReturnType(): bool {}

    public function getTentativeReturnType(): ?ReflectionType {}

    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionFunction extends ReflectionFunctionAbstract
{
    /** @cvalue ZEND_ACC_DEPRECATED */
    public const int IS_DEPRECATED = UNKNOWN;

    public function __construct(Closure|string $function) {}

    public function __toString(): string {}

    public function isAnonymous(): bool {}

    /**
     * @tentative-return-type
     * @deprecated ReflectionFunction can no longer be constructed for disabled functions
     */
    public function isDisabled(): bool {}

    /** @tentative-return-type */
    public function invoke(mixed ...$args): mixed {}

    /** @tentative-return-type */
    public function invokeArgs(array $args): mixed {}

    /** @tentative-return-type */
    public function getClosure(): Closure {}
}

/** @not-serializable */
final class ReflectionGenerator
{
    public function __construct(Generator $generator) {}

    /** @tentative-return-type */
    public function getExecutingLine(): int {}

    /** @tentative-return-type */
    public function getExecutingFile(): string {}

    /** @tentative-return-type */
    public function getTrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT): array {}

    /** @tentative-return-type */
    public function getFunction(): ReflectionFunctionAbstract {}

    /** @tentative-return-type */
    public function getThis(): ?object {}

    /** @tentative-return-type */
    public function getExecutingGenerator(): Generator {}
}

class ReflectionMethod extends ReflectionFunctionAbstract
{
    /** @cvalue ZEND_ACC_STATIC */
    public const int IS_STATIC = UNKNOWN;
    /** @cvalue ZEND_ACC_PUBLIC */
    public const int IS_PUBLIC = UNKNOWN;
    /** @cvalue ZEND_ACC_PROTECTED */
    public const int IS_PROTECTED = UNKNOWN;
    /** @cvalue ZEND_ACC_PRIVATE */
    public const int IS_PRIVATE = UNKNOWN;
    /** @cvalue ZEND_ACC_ABSTRACT */
    public const int IS_ABSTRACT = UNKNOWN;
    /** @cvalue ZEND_ACC_FINAL */
    public const int IS_FINAL = UNKNOWN;

    public string $name;
    public string $class;

    public function __construct(object|string $objectOrMethod, ?string $method = null) {}

    public static function createFromMethodName(string $method): static {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function isPublic(): bool {}

    /** @tentative-return-type */
    public function isPrivate(): bool {}

    /** @tentative-return-type */
    public function isProtected(): bool {}

    /** @tentative-return-type */
    public function isAbstract(): bool {}

    /** @tentative-return-type */
    public function isFinal(): bool {}

    /** @tentative-return-type */
    public function isConstructor(): bool {}

    /** @tentative-return-type */
    public function isDestructor(): bool {}

    /** @tentative-return-type */
    public function getClosure(?object $object = null): Closure {}

    /** @tentative-return-type */
    public function getModifiers(): int {}

    /** @tentative-return-type */
    public function invoke(?object $object, mixed ...$args): mixed {}

    /** @tentative-return-type */
    public function invokeArgs(?object $object, array $args): mixed {}

    /** @tentative-return-type */
    public function getDeclaringClass(): ReflectionClass {}

    /** @tentative-return-type */
    public function getPrototype(): ReflectionMethod {}

    public function hasPrototype(): bool {}

    /** @tentative-return-type */
    public function setAccessible(bool $accessible): void {}
}

/** @not-serializable */
class ReflectionClass implements Reflector
{
    /**
     * @cvalue ZEND_ACC_IMPLICIT_ABSTRACT_CLASS
     * @todo deprecate
     */
    public const int IS_IMPLICIT_ABSTRACT = UNKNOWN;
    /** @cvalue ZEND_ACC_EXPLICIT_ABSTRACT_CLASS */
    public const int IS_EXPLICIT_ABSTRACT = UNKNOWN;
    /** @cvalue ZEND_ACC_FINAL */
    public const int IS_FINAL = UNKNOWN;
    /** @cvalue ZEND_ACC_READONLY_CLASS */
    public const int IS_READONLY = UNKNOWN;

    public string $name;

    private function __clone(): void {}

    public function __construct(object|string $objectOrClass) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function isInternal(): bool {}

    /** @tentative-return-type */
    public function isUserDefined(): bool {}

    /** @tentative-return-type */
    public function isAnonymous(): bool {}

    /** @tentative-return-type */
    public function isInstantiable(): bool {}

    /** @tentative-return-type */
    public function isCloneable(): bool {}

    /** @tentative-return-type */
    public function getFileName(): string|false {}

    /** @tentative-return-type */
    public function getStartLine(): int|false {}

    /** @tentative-return-type */
    public function getEndLine(): int|false {}

    /** @tentative-return-type */
    public function getDocComment(): string|false {}

    /** @tentative-return-type */
    public function getConstructor(): ?ReflectionMethod {}

    /** @tentative-return-type */
    public function hasMethod(string $name): bool {}

    /** @tentative-return-type */
    public function getMethod(string $name): ReflectionMethod {}

    /** @tentative-return-type */
    public function getMethods(?int $filter = null): array {}

    /** @tentative-return-type */
    public function hasProperty(string $name): bool {}

    /** @tentative-return-type */
    public function getProperty(string $name): ReflectionProperty {}

    /** @tentative-return-type */
    public function getProperties(?int $filter = null): array {}

    /** @tentative-return-type */
    public function hasConstant(string $name): bool {}

    /** @tentative-return-type */
    public function getConstants(?int $filter = null): array {}

    /** @tentative-return-type */
    public function getReflectionConstants(?int $filter = null): array {}

    /** @tentative-return-type */
    public function getConstant(string $name): mixed {} // TODO throw exception when the constant doesn't exist

    /** @tentative-return-type */
    public function getReflectionConstant(string $name): ReflectionClassConstant|false {}

    /** @tentative-return-type */
    public function getInterfaces(): array {}

    /** @tentative-return-type */
    public function getInterfaceNames(): array {}

    /** @tentative-return-type */
    public function isInterface(): bool {}

    /** @tentative-return-type */
    public function getTraits(): array {}

    /** @tentative-return-type */
    public function getTraitNames(): array {}

    /** @tentative-return-type */
    public function getTraitAliases(): array {}

    /** @tentative-return-type */
    public function isTrait(): bool {}

    public function isEnum(): bool {}

    /** @tentative-return-type */
    public function isAbstract(): bool {}

    /** @tentative-return-type */
    public function isFinal(): bool {}

    public function isReadOnly(): bool {}

    /** @tentative-return-type */
    public function getModifiers(): int {}

    /** @tentative-return-type */
    public function isInstance(object $object): bool {}

    /** @tentative-return-type */
    public function newInstance(mixed ...$args): object {}

    /** @tentative-return-type */
    public function newInstanceWithoutConstructor(): object {}

    /** @tentative-return-type */
    public function newInstanceArgs(array $args = []): ?object {}

    /** @tentative-return-type */
    public function getParentClass(): ReflectionClass|false {}

    /** @tentative-return-type */
    public function isSubclassOf(ReflectionClass|string $class): bool {}

    /** @tentative-return-type */
    public function getStaticProperties(): array {}

    /** @tentative-return-type */
    public function getStaticPropertyValue(string $name, mixed $default = UNKNOWN): mixed {}

    /** @tentative-return-type */
    public function setStaticPropertyValue(string $name, mixed $value): void {}

    /** @tentative-return-type */
    public function getDefaultProperties(): array {}

    /** @tentative-return-type */
    public function isIterable(): bool {}

    /**
     * @tentative-return-type
     * @alias ReflectionClass::isIterable
     */
    public function isIterateable(): bool {}

    /** @tentative-return-type */
    public function implementsInterface(ReflectionClass|string $interface): bool {}

    /** @tentative-return-type */
    public function getExtension(): ?ReflectionExtension {}

    /** @tentative-return-type */
    public function getExtensionName(): string|false {}

    /** @tentative-return-type */
    public function inNamespace(): bool {}

    /** @tentative-return-type */
    public function getNamespaceName(): string {}

    /** @tentative-return-type */
    public function getShortName(): string {}

    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

class ReflectionObject extends ReflectionClass
{
    public function __construct(object $object) {}
}

/** @not-serializable */
class ReflectionProperty implements Reflector
{
    /** @cvalue ZEND_ACC_STATIC */
    public const int IS_STATIC = UNKNOWN;
    /** @cvalue ZEND_ACC_READONLY */
    public const int IS_READONLY = UNKNOWN;
    /** @cvalue ZEND_ACC_PUBLIC */
    public const int IS_PUBLIC = UNKNOWN;
    /** @cvalue ZEND_ACC_PROTECTED */
    public const int IS_PROTECTED = UNKNOWN;
    /** @cvalue ZEND_ACC_PRIVATE */
    public const int IS_PRIVATE = UNKNOWN;

    public string $name;
    public string $class;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    public function __construct(object|string $class, string $property) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function getValue(?object $object = null): mixed {}

    /** @tentative-return-type */
    public function setValue(mixed $objectOrValue, mixed $value = UNKNOWN): void {}

    /** @tentative-return-type */
    public function isInitialized(?object $object = null): bool {}

    /** @tentative-return-type */
    public function isPublic(): bool {}

    /** @tentative-return-type */
    public function isPrivate(): bool {}

    /** @tentative-return-type */
    public function isProtected(): bool {}

    /** @tentative-return-type */
    public function isStatic(): bool {}

    public function isReadOnly(): bool {}

    /** @tentative-return-type */
    public function isDefault(): bool {}

    public function isPromoted(): bool {}

    /** @tentative-return-type */
    public function getModifiers(): int {}

    /** @tentative-return-type */
    public function getDeclaringClass(): ReflectionClass {}

    /** @tentative-return-type */
    public function getDocComment(): string|false {}

    /** @tentative-return-type */
    public function setAccessible(bool $accessible): void {}

    /** @tentative-return-type */
    public function getType(): ?ReflectionType {}

    /** @tentative-return-type */
    public function hasType(): bool {}

    public function hasDefaultValue(): bool {}

    /** @tentative-return-type */
    public function getDefaultValue(): mixed {}

    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

/** @not-serializable */
class ReflectionClassConstant implements Reflector
{
    /** @cvalue ZEND_ACC_PUBLIC */
    public const int IS_PUBLIC = UNKNOWN;
    /** @cvalue ZEND_ACC_PROTECTED */
    public const int IS_PROTECTED = UNKNOWN;
    /** @cvalue ZEND_ACC_PRIVATE */
    public const int IS_PRIVATE = UNKNOWN;
    /** @cvalue ZEND_ACC_FINAL */
    public const int IS_FINAL = UNKNOWN;

    public string $name;
    public string $class;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    public function __construct(object|string $class, string $constant) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function getValue(): mixed {}

    /** @tentative-return-type */
    public function isPublic(): bool {}

    /** @tentative-return-type */
    public function isPrivate(): bool {}

    /** @tentative-return-type */
    public function isProtected(): bool {}

    public function isFinal(): bool {}

    /** @tentative-return-type */
    public function getModifiers(): int {}

    /** @tentative-return-type */
    public function getDeclaringClass(): ReflectionClass {}

    /** @tentative-return-type */
    public function getDocComment(): string|false {}

    public function getAttributes(?string $name = null, int $flags = 0): array {}

    public function isEnumCase(): bool {}

    public function hasType(): bool {}

    public function getType(): ?ReflectionType {}
}

/** @not-serializable */
class ReflectionParameter implements Reflector
{
    public string $name;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    /** @param string|array|object $function */
    public function __construct($function, int|string $param) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function isPassedByReference(): bool {}

    /** @tentative-return-type */
    public function canBePassedByValue(): bool {}

    /** @tentative-return-type */
    public function getDeclaringFunction(): ReflectionFunctionAbstract {}

    /** @tentative-return-type */
    public function getDeclaringClass(): ?ReflectionClass {}

    /**
     * @tentative-return-type
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function getClass(): ?ReflectionClass {}

    /** @tentative-return-type */
    public function hasType(): bool {}

    /** @tentative-return-type */
    public function getType(): ?ReflectionType {}

    /**
     * @tentative-return-type
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function isArray(): bool {}

    /**
     * @tentative-return-type
     * @deprecated Use ReflectionParameter::getType() instead
     */
    public function isCallable(): bool {}

    /** @tentative-return-type */
    public function allowsNull(): bool {}

    /** @tentative-return-type */
    public function getPosition(): int {}

    /** @tentative-return-type */
    public function isOptional(): bool {}

    /** @tentative-return-type */
    public function isDefaultValueAvailable(): bool {}

    /** @tentative-return-type */
    public function getDefaultValue(): mixed {}

    /** @tentative-return-type */
    public function isDefaultValueConstant(): bool {}

    /** @tentative-return-type */
    public function getDefaultValueConstantName(): ?string {}

    /** @tentative-return-type */
    public function isVariadic(): bool {}

    public function isPromoted(): bool {}

    public function getAttributes(?string $name = null, int $flags = 0): array {}
}

/** @not-serializable */
abstract class ReflectionType implements Stringable
{
    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    /** @tentative-return-type */
    public function allowsNull(): bool {}

    public function __toString(): string {}
}

class ReflectionNamedType extends ReflectionType
{
    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function isBuiltin(): bool {}
}

class ReflectionUnionType extends ReflectionType
{
    public function getTypes(): array {}
}

class ReflectionIntersectionType extends ReflectionType
{
    public function getTypes(): array {}
}

/** @not-serializable */
class ReflectionExtension implements Reflector
{
    public string $name;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    public function __construct(string $name) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function getVersion(): ?string {}

    /** @tentative-return-type */
    public function getFunctions(): array {}

    /** @tentative-return-type */
    public function getConstants(): array {}

    /** @tentative-return-type */
    public function getINIEntries(): array {}

    /** @tentative-return-type */
    public function getClasses(): array {}

    /** @tentative-return-type */
    public function getClassNames(): array {}

    /** @tentative-return-type */
    public function getDependencies(): array {}

    /** @tentative-return-type */
    public function info(): void {}

    /** @tentative-return-type */
    public function isPersistent(): bool {}

    /** @tentative-return-type */
    public function isTemporary(): bool {}
}

/** @not-serializable */
class ReflectionZendExtension implements Reflector
{
    public string $name;

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    public function __construct(string $name) {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getName(): string {}

    /** @tentative-return-type */
    public function getVersion(): string {}

    /** @tentative-return-type */
    public function getAuthor(): string {}

    /** @tentative-return-type */
    public function getURL(): string {}

    /** @tentative-return-type */
    public function getCopyright(): string {}
}

/** @not-serializable */
final class ReflectionReference
{
    public static function fromArrayElement(array $array, int|string $key): ?ReflectionReference {}

    public function getId(): string {}

    /** @implementation-alias ReflectionClass::__clone */
    private function __clone(): void {}

    private function __construct() {}
}

/** @not-serializable */
class ReflectionAttribute implements Reflector
{
    /** @cvalue REFLECTION_ATTRIBUTE_IS_INSTANCEOF */
    public const int IS_INSTANCEOF = UNKNOWN;

    public function getName(): string {}
    public function getTarget(): int {}
    public function isRepeated(): bool {}
    public function getArguments(): array {}
    public function newInstance(): object {}

    public function __toString(): string {}

    private function __clone(): void {}

    private function __construct() {}
}

class ReflectionEnum extends ReflectionClass
{
    public function __construct(object|string $objectOrClass) {}

    public function hasCase(string $name): bool {}

    public function getCase(string $name): ReflectionEnumUnitCase {}

    public function getCases(): array {}

    public function isBacked(): bool {}

    public function getBackingType(): ?ReflectionNamedType {}
}

class ReflectionEnumUnitCase extends ReflectionClassConstant
{
    public function __construct(object|string $class, string $constant) {}

    public function getEnum(): ReflectionEnum {}

    /**
     * @implementation-alias ReflectionClassConstant::getValue
     * @no-verify
     */
    public function getValue(): UnitEnum {}
}

class ReflectionEnumBackedCase extends ReflectionEnumUnitCase
{
    public function __construct(object|string $class, string $constant) {}

    public function getBackingValue(): int|string {}
}

/** @not-serializable */
final class ReflectionFiber
{
    public function __construct(Fiber $fiber) {}

    public function getFiber(): Fiber {}

    public function getExecutingFile(): ?string {}

    public function getExecutingLine(): ?int {}

    public function getCallable(): callable {}

    public function getTrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT): array {}
}
