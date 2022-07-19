<?php

/** @generate-class-entries */

#[Attribute(Attribute::TARGET_CLASS)]
final class Attribute
{
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_CLASS
     */
    const TARGET_CLASS = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_FUNCTION
     */
    const TARGET_FUNCTION = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_METHOD
     */
    const TARGET_METHOD = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_PROPERTY
     */
    const TARGET_PROPERTY = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_CLASS_CONST
     */
    const TARGET_CLASS_CONSTANT = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_PARAMETER
     */
    const TARGET_PARAMETER = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_TARGET_ALL
     */
    const TARGET_ALL = UNKNOWN;
    /**
     * @var int
     * @cvalue ZEND_ATTRIBUTE_IS_REPEATABLE
     */
    const IS_REPEATABLE = UNKNOWN;

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
