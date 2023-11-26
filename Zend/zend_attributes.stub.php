<?php

/** @generate-class-entries */

#[Attribute(Attribute::TARGET_CLASS)]
final class Attribute
{
    /** @cvalue ZEND_ATTRIBUTE_TARGET_CLASS */
    const int TARGET_CLASS = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_FUNCTION */
    const int TARGET_FUNCTION = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_METHOD */
    const int TARGET_METHOD = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_PROPERTY */
    const int TARGET_PROPERTY = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_CLASS_CONST */
    const int TARGET_CLASS_CONSTANT = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_PARAMETER */
    const int TARGET_PARAMETER = UNKNOWN;
    /** @cvalue ZEND_ATTRIBUTE_TARGET_ALL */
    const int TARGET_ALL = UNKNOWN;
    /**  @cvalue ZEND_ATTRIBUTE_IS_REPEATABLE */
    const int IS_REPEATABLE = UNKNOWN;

    public int $flags;

    public function __construct(int $flags = Attribute::TARGET_ALL) {}
}

#[Attribute(Attribute::TARGET_METHOD)]
final class ReturnTypeWillChange
{
    public function __construct() {}
}

#[Attribute(Attribute::TARGET_CLASS)]
final class AllowDynamicProperties
{
    public function __construct() {}
}

/**
 * @strict-properties
 */
#[Attribute(Attribute::TARGET_PARAMETER)]
final class SensitiveParameter
{
    public function __construct() {}
}

/**
 * @strict-properties
 * @not-serializable
 */
final class SensitiveParameterValue
{
    private readonly mixed $value;

    public function __construct(mixed $value) {}

    public function getValue(): mixed {}

    public function __debugInfo(): array {}
}

/**
 * @strict-properties
 */
#[Attribute(Attribute::TARGET_METHOD)]
final class Override
{
    public function __construct() {}
}

/**
 * @strict-properties
 */
#[Attribute(Attribute::TARGET_CLASS)]
final class NotSerializable
{
    public function __construct() {}
}
