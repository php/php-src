<?php

/** @generate-class-entries */

/**
 * @not-serializable
 * @strict-properties
 */
final class IntlNumberFormatter {
    private function __construct() {}

    public static function createFromSkeleton(string $skeleton, string $locale): ?IntlNumberFormatter {}

    public function format(float|int|string $value): string|false {}

    public function getErrorCode(): int {}

    public function getErrorMessage(): string {}
}
