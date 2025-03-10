<?php

/** @generate-class-entries */

interface UnitEnum
{
    public static function cases(): array;
}

interface BackedEnum extends UnitEnum
{
    public static function from(never $value): static;

    public static function tryFrom(never $value): ?static;
}
