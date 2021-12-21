<?php

/** @generate-class-entries */

final class Attribute
{
    public int $flags;

    public function __construct(int $flags = Attribute::TARGET_ALL) {}
}

final class ReturnTypeWillChange
{
    public function __construct() {}
}

final class AllowDynamicProperties
{
    public function __construct() {}
}

/**
 * @strict-properties
 */
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
