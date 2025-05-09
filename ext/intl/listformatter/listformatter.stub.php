<?php

/** @generate-class-entries */

/**
 * @not-serializable
 * @strict-properties
 */
final class IntlListFormatter {

    /** @cvalue ULISTFMT_TYPE_OR */
    public const int TYPE_OR = UNKNOWN;

    /** @cvalue ULISTFMT_TYPE_UNITS */
    public const int TYPE_UNITS = UNKNOWN;

    /** @cvalue ULISTFMT_TYPE_AND */
    public const int TYPE_AND = UNKNOWN;

    /** @cvalue ULISTFMT_WIDTH_WIDE */
    public const int WIDTH_WIDE = UNKNOWN;

    /** @cvalue ULISTFMT_WIDTH_SHORT */
    public const int WIDTH_SHORT = UNKNOWN;

    /** @cvalue ULISTFMT_WIDTH_NARROW */
    public const int WIDTH_NARROW = UNKNOWN;

    public function __construct(string $locale, int $type = IntlListFormatter::TYPE_AND, int $width = IntlListFormatter::WIDTH_WIDE) {}

    public function format(array $strings): string|false {}

    public function getErrorCode(): int {}

    public function getErrorMessage(): string {}
}
