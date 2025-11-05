<?php

/** @generate-class-entries */

interface UnitEnum
{
    public static function cases(): array;
}

interface BackedEnum extends UnitEnum
{
    public static function from(int|string $value): static;

    public static function tryFrom(int|string $value): ?static;

    /**
     * Returns an indexed array of all backing values for the enum cases.
     *
     * @return int[]|string[]
     */
    public static function values(): array;
}
