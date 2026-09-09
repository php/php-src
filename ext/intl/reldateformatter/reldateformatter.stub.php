<?php

/** @generate-class-entries */

/**
 * @not-serializable
 * @strict-properties
 */
final class IntlRelativeDateTimeFormatter
{
    /** @cvalue UDAT_STYLE_LONG */
    public const int STYLE_LONG = UNKNOWN;

    /** @cvalue UDAT_STYLE_SHORT */
    public const int STYLE_SHORT = UNKNOWN;

    /** @cvalue UDAT_STYLE_NARROW */
    public const int STYLE_NARROW = UNKNOWN;

    /** @cvalue UDISPCTX_CAPITALIZATION_NONE */
    public const int CAPITALIZATION_NONE = UNKNOWN;

    /** @cvalue UDISPCTX_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE */
    public const int CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE = UNKNOWN;

    /** @cvalue UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE */
    public const int CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE = UNKNOWN;

    /** @cvalue UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU */
    public const int CAPITALIZATION_FOR_UI_LIST_OR_MENU = UNKNOWN;

    /** @cvalue UDISPCTX_CAPITALIZATION_FOR_STANDALONE */
    public const int CAPITALIZATION_FOR_STANDALONE = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_YEAR */
    public const int UNIT_YEAR = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_QUARTER */
    public const int UNIT_QUARTER = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_MONTH */
    public const int UNIT_MONTH = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_WEEK */
    public const int UNIT_WEEK = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_DAY */
    public const int UNIT_DAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_HOUR */
    public const int UNIT_HOUR = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_MINUTE */
    public const int UNIT_MINUTE = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_SECOND */
    public const int UNIT_SECOND = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_SUNDAY */
    public const int UNIT_SUNDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_MONDAY */
    public const int UNIT_MONDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_TUESDAY */
    public const int UNIT_TUESDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_WEDNESDAY */
    public const int UNIT_WEDNESDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_THURSDAY */
    public const int UNIT_THURSDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_FRIDAY */
    public const int UNIT_FRIDAY = UNKNOWN;

    /** @cvalue UDAT_REL_UNIT_SATURDAY */
    public const int UNIT_SATURDAY = UNKNOWN;

    public function __construct(
        ?string $locale = null,
        int $style = IntlRelativeDateTimeFormatter::STYLE_LONG,
        int $capitalizationContext = IntlRelativeDateTimeFormatter::CAPITALIZATION_NONE,
        ?NumberFormatter $numberFormatter = null,
    ) {}

    public function format(int|float $offset, int $unit): string|false {}

    public function formatNumeric(int|float $offset, int $unit): string|false {}

    public function combineDateAndTime(string $relativeDate, string $time): string|false {}

    public function getErrorCode(): int {}

    public function getErrorMessage(): string {}
}
