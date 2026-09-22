<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

enum IntlRelativeDateTimeFormatterStyle
{
    case Long;
    case Short;
    case Narrow;
}

enum IntlRelativeDateTimeFormatterCapitalization
{
    case None;
    case MiddleOfSentence;
    case BeginningOfSentence;
    case UiListAndMenu;
    case Standalone;
}

enum IntlRelativeDateTimeFormatterUnit
{
    case Year;
    case Quarter;
    case Month;
    case Week;
    case Day;
    case Hour;
    case Minute;
    case Second;
    case Sunday;
    case Monday;
    case Tuesday;
    case Wednesday;
    case Thursday;
    case Friday;
    case Saturday;
}

/**
 * @not-serializable
 * @strict-properties
 */
final class IntlRelativeDateTimeFormatter
{
    public function __construct(
        ?string $locale = null,
        IntlRelativeDateTimeFormatterStyle $style = IntlRelativeDateTimeFormatterStyle::Long,
        IntlRelativeDateTimeFormatterCapitalization $capitalizationContext = IntlRelativeDateTimeFormatterCapitalization::None,
        ?NumberFormatter $numberFormatter = null,
    ) {}

    public function format(int|float $offset, IntlRelativeDateTimeFormatterUnit $unit): string|false {}

    public function formatNumeric(int|float $offset, IntlRelativeDateTimeFormatterUnit $unit): string|false {}

    public function combineDateAndTime(string $relativeDate, string $time): string|false {}

    public function getErrorCode(): int {}

    public function getErrorMessage(): string {}
}
