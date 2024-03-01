<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue INTL_MAX_LOCALE_LEN
 */
const INTL_MAX_LOCALE_LEN = UNKNOWN;
/**
 * @var string
 * @cvalue U_ICU_VERSION
 */
const INTL_ICU_VERSION = UNKNOWN;
#ifdef U_ICU_DATA_VERSION
/**
 * @var string
 * @cvalue U_ICU_DATA_VERSION
 */
const INTL_ICU_DATA_VERSION = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue GRAPHEME_EXTRACT_TYPE_COUNT
 */
const GRAPHEME_EXTR_COUNT = UNKNOWN;
/**
 * @var int
 * @cvalue GRAPHEME_EXTRACT_TYPE_MAXBYTES
 */
const GRAPHEME_EXTR_MAXBYTES = UNKNOWN;
/**
 * @var int
 * @cvalue GRAPHEME_EXTRACT_TYPE_MAXCHARS
 */
const GRAPHEME_EXTR_MAXCHARS = UNKNOWN;

/**
 * Option to prohibit processing of unassigned codepoints in the input and
 * do not check if the input conforms to STD-3 ASCII rules.
 * @var int
 * @cvalue UIDNA_DEFAULT
 */
const IDNA_DEFAULT = UNKNOWN;

/**
 * Option to allow processing of unassigned codepoints in the input
 * @var int
 * @cvalue UIDNA_ALLOW_UNASSIGNED
 */
const IDNA_ALLOW_UNASSIGNED = UNKNOWN;

/**
 * Option to check if input conforms to STD-3 ASCII rules
 * @var int
 * @cvalue UIDNA_USE_STD3_RULES
 */
const IDNA_USE_STD3_RULES = UNKNOWN;

/**
 * Option to check for whether the input conforms to the BiDi rules.
 * Ignored by the IDNA2003 implementation. (IDNA2003 always performs a BiDi check.)
 * @var int
 * @cvalue UIDNA_CHECK_BIDI
 */
const IDNA_CHECK_BIDI = UNKNOWN;

/**
 * Option to check for whether the input conforms to the CONTEXTJ rules.
 * Ignored by the IDNA2003 implementation. (The CONTEXTJ check is new in IDNA2008.)
 * @var int
 * @cvalue UIDNA_CHECK_CONTEXTJ
 */
const IDNA_CHECK_CONTEXTJ = UNKNOWN;

/**
 * Option for nontransitional processing in ToASCII().
 * By default, ToASCII() uses transitional processing.
 * Ignored by the IDNA2003 implementation.
 * @var int
 * @cvalue UIDNA_NONTRANSITIONAL_TO_ASCII
 */
const IDNA_NONTRANSITIONAL_TO_ASCII = UNKNOWN;

/**
 * Option for nontransitional processing in ToUnicode().
 * By default, ToUnicode() uses transitional processing.
 * Ignored by the IDNA2003 implementation.
 * @var int
 * @cvalue UIDNA_NONTRANSITIONAL_TO_UNICODE
 */
const IDNA_NONTRANSITIONAL_TO_UNICODE = UNKNOWN;

/* VARIANTS */

/**
 * @var int
 * @cvalue INTL_IDN_VARIANT_UTS46
 */
const INTL_IDNA_VARIANT_UTS46 = UNKNOWN;

/* PINFO ERROR CODES */
/**
 * @var int
 * @cvalue UIDNA_ERROR_EMPTY_LABEL
 */
const IDNA_ERROR_EMPTY_LABEL = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_LABEL_TOO_LONG
 */
const IDNA_ERROR_LABEL_TOO_LONG = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_DOMAIN_NAME_TOO_LONG
 */
const IDNA_ERROR_DOMAIN_NAME_TOO_LONG = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_LEADING_HYPHEN
 */
const IDNA_ERROR_LEADING_HYPHEN = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_TRAILING_HYPHEN
 */
const IDNA_ERROR_TRAILING_HYPHEN = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_HYPHEN_3_4
 */
const IDNA_ERROR_HYPHEN_3_4 = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_LEADING_COMBINING_MARK
 */
const IDNA_ERROR_LEADING_COMBINING_MARK = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_DISALLOWED
 */
const IDNA_ERROR_DISALLOWED = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_PUNYCODE
 */
const IDNA_ERROR_PUNYCODE = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_LABEL_HAS_DOT
 */
const IDNA_ERROR_LABEL_HAS_DOT = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_INVALID_ACE_LABEL
 */
const IDNA_ERROR_INVALID_ACE_LABEL = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_BIDI
 */
const IDNA_ERROR_BIDI = UNKNOWN;
/**
 * @var int
 * @cvalue UIDNA_ERROR_CONTEXTJ
 */
const IDNA_ERROR_CONTEXTJ = UNKNOWN;

class IntlException extends Exception
{
}

/* calendar */

/** @param IntlTimeZone|DateTimeZone|string|null $timezone */
function intlcal_create_instance($timezone = null, ?string $locale = null): ?IntlCalendar {}

function intlcal_get_keyword_values_for_locale(string $keyword, string $locale, bool $onlyCommon): IntlIterator|false {}

function intlcal_get_now(): float {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function intlcal_get_available_locales(): array {}

function intlcal_get(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_time(IntlCalendar $calendar): float|false {}

function intlcal_set_time(IntlCalendar $calendar, float $timestamp): bool {}

function intlcal_add(IntlCalendar $calendar, int $field, int $value): bool {}

/** @param IntlTimeZone|DateTimeZone|string|null $timezone */
function intlcal_set_time_zone(IntlCalendar $calendar, $timezone): bool {}

function intlcal_after(IntlCalendar $calendar, IntlCalendar $other): bool {}

function intlcal_before(IntlCalendar $calendar, IntlCalendar $other): bool {}

/** @deprecated */
function intlcal_set(IntlCalendar $calendar, int $year, int $month, int $dayOfMonth = UNKNOWN, int $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN): true {}

/** @param int|bool $value */
function intlcal_roll(IntlCalendar $calendar, int $field, $value): bool {}

function intlcal_clear(IntlCalendar $calendar, ?int $field = null): true {}

function intlcal_field_difference(IntlCalendar $calendar, float $timestamp, int $field): int|false {}

function intlcal_get_actual_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_actual_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_day_of_week_type(IntlCalendar $calendar, int $dayOfWeek): int|false {}

function intlcal_get_first_day_of_week(IntlCalendar $calendar): int|false {}

function intlcal_get_least_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_greatest_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_locale(IntlCalendar $calendar, int $type): string|false {}

function intlcal_get_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_minimal_days_in_first_week(IntlCalendar $calendar): int|false {}

function intlcal_set_minimal_days_in_first_week(IntlCalendar $calendar, int $days): true {}

function intlcal_get_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_time_zone(IntlCalendar $calendar): IntlTimeZone|false {}

function intlcal_get_type(IntlCalendar $calendar): string {}

function intlcal_get_weekend_transition(IntlCalendar $calendar, int $dayOfWeek): int|false {}

function intlcal_in_daylight_time(IntlCalendar $calendar): bool {}

function intlcal_is_lenient(IntlCalendar $calendar): bool {}

function intlcal_is_set(IntlCalendar $calendar, int $field): bool {}

function intlcal_is_equivalent_to(IntlCalendar $calendar, IntlCalendar $other): bool {}

function intlcal_is_weekend(IntlCalendar $calendar, ?float $timestamp = null): bool {}

function intlcal_set_first_day_of_week(IntlCalendar $calendar, int $dayOfWeek): true {}

function intlcal_set_lenient(IntlCalendar $calendar, bool $lenient): true {}

function intlcal_get_repeated_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_equals(IntlCalendar $calendar, IntlCalendar $other): bool {}

function intlcal_get_skipped_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_set_repeated_wall_time_option(IntlCalendar $calendar, int $option): true {}

function intlcal_set_skipped_wall_time_option(IntlCalendar $calendar, int $option): true {}

function intlcal_from_date_time(DateTime|string $datetime, ?string $locale = null): ?IntlCalendar {}

function intlcal_to_date_time(IntlCalendar $calendar): DateTime|false {}

function intlcal_get_error_code(IntlCalendar $calendar): int|false {}

function intlcal_get_error_message(IntlCalendar $calendar): string|false {}

/**
 * @param DateTimeZone|IntlTimeZone|string|int|null $timezoneOrYear
 * @param string|int|null $localeOrMonth
 * @param int $day
 * @param int $hour
 * @param int $minute
 * @param int $second
 * @deprecated
 */
function intlgregcal_create_instance($timezoneOrYear = UNKNOWN, $localeOrMonth = UNKNOWN, $day = UNKNOWN, $hour = UNKNOWN, $minute = UNKNOWN, $second = UNKNOWN): ?IntlGregorianCalendar {}

function intlgregcal_set_gregorian_change(IntlGregorianCalendar $calendar, float $timestamp): bool {}

function intlgregcal_get_gregorian_change(IntlGregorianCalendar $calendar): float {}

function intlgregcal_is_leap_year(IntlGregorianCalendar $calendar, int $year): bool {}

/* collator */

function collator_create(string $locale): ?Collator {}

function collator_compare(Collator $object, string $string1, string $string2): int|false {}

function collator_get_attribute(Collator $object, int $attribute): int|false {}

function collator_set_attribute(Collator $object, int $attribute, int $value): bool {}

function collator_get_strength(Collator $object): int {}

function collator_set_strength(Collator $object, int $strength): bool {}

function collator_sort(Collator $object, array &$array, int $flags = Collator::SORT_REGULAR): bool {}

function collator_sort_with_sort_keys(Collator $object, array &$array): bool {}

function collator_asort(Collator $object, array &$array, int $flags = Collator::SORT_REGULAR): bool {}

function collator_get_locale(Collator $object, int $type): string|false {}

function collator_get_error_code(Collator $object): int|false {}

function collator_get_error_message(Collator $object): string|false {}

function collator_get_sort_key(Collator $object, string $string): string|false {}

/* common */

function intl_get_error_code(): int {}

function intl_get_error_message(): string {}

function intl_is_failure(int $errorCode): bool {}

function intl_error_name(int $errorCode): string {}

/* dateformat */

/** @param IntlTimeZone|DateTimeZone|string|null $timezone */
function datefmt_create(
    ?string $locale,
    int $dateType = IntlDateFormatter::FULL,
    int $timeType = IntlDateFormatter::FULL,
    $timezone = null,
    IntlCalendar|int|null $calendar = null,
    ?string $pattern = null
): ?IntlDateFormatter {}

function datefmt_get_datetype(IntlDateFormatter $formatter): int|false {}

function datefmt_get_timetype(IntlDateFormatter $formatter): int|false {}

function datefmt_get_calendar(IntlDateFormatter $formatter): int|false {}

function datefmt_set_calendar(IntlDateFormatter $formatter, IntlCalendar|int|null $calendar): bool {}

function datefmt_get_timezone_id(IntlDateFormatter $formatter): string|false {}

function datefmt_get_calendar_object(IntlDateFormatter $formatter): IntlCalendar|false|null {}

function datefmt_get_timezone(IntlDateFormatter $formatter): IntlTimeZone|false {}

/** @param IntlTimeZone|DateTimeZone|string|null $timezone */
function datefmt_set_timezone(IntlDateFormatter $formatter, $timezone): bool {}

function datefmt_set_pattern(IntlDateFormatter $formatter, string $pattern): bool {}

function datefmt_get_pattern(IntlDateFormatter $formatter): string|false {}

function datefmt_get_locale(IntlDateFormatter $formatter, int $type = ULOC_ACTUAL_LOCALE): string|false {}

function datefmt_set_lenient(IntlDateFormatter $formatter, bool $lenient): void {}

function datefmt_is_lenient(IntlDateFormatter $formatter): bool {}

/** @param IntlCalendar|DateTimeInterface|array|string|int|float $datetime */
function datefmt_format(IntlDateFormatter $formatter, $datetime): string|false {}

/**
 * @param IntlCalendar|DateTimeInterface $datetime
 * @param array|int|string|null $format
 */
function datefmt_format_object($datetime, $format = null, ?string $locale = null): string|false {}

/** @param int $offset */
function datefmt_parse(IntlDateFormatter $formatter, string $string, &$offset = null): int|float|false {}

/**
 * @param int $offset
 * @return array<string, int>|false
 * @refcount 1
 */
function datefmt_localtime(IntlDateFormatter $formatter, string $string, &$offset = null): array|false {}

function datefmt_get_error_code(IntlDateFormatter $formatter): int {}

function datefmt_get_error_message(IntlDateFormatter $formatter): string {}

/* formatter */

function numfmt_create(string $locale, int $style, ?string $pattern = null): ?NumberFormatter {}

function numfmt_format(NumberFormatter $formatter, int|float $num, int $type = NumberFormatter::TYPE_DEFAULT): string|false {}

/** @param int $offset */
function numfmt_parse(NumberFormatter $formatter, string $string, int $type = NumberFormatter::TYPE_DOUBLE, &$offset = null): int|float|false {}

function numfmt_format_currency(NumberFormatter $formatter, float $amount, string $currency): string|false {}

/**
 * @param string $currency
 * @param int $offset
 */
function numfmt_parse_currency(NumberFormatter $formatter, string $string, &$currency, &$offset = null): float|false {}

function numfmt_set_attribute(NumberFormatter $formatter, int $attribute, int|float $value): bool {}

function numfmt_get_attribute(NumberFormatter $formatter, int $attribute): int|float|false {}

function numfmt_set_text_attribute(NumberFormatter $formatter, int $attribute, string $value): bool {}

function numfmt_get_text_attribute(NumberFormatter $formatter, int $attribute): string|false {}

function numfmt_set_symbol(NumberFormatter $formatter, int $symbol, string $value): bool {}

function numfmt_get_symbol(NumberFormatter $formatter, int $symbol): string|false {}

function numfmt_set_pattern(NumberFormatter $formatter, string $pattern): bool {}

function numfmt_get_pattern(NumberFormatter $formatter): string|false {}

function numfmt_get_locale(NumberFormatter $formatter, int $type = ULOC_ACTUAL_LOCALE): string|false {}

function numfmt_get_error_code(NumberFormatter $formatter): int {}

function numfmt_get_error_message(NumberFormatter $formatter): string {}

/* grapheme */

function grapheme_strlen(string $string): int|false|null {}

function grapheme_strpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_stripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strrpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_substr(string $string, int $offset, ?int $length = null): string|false {}

function grapheme_strstr(string $haystack, string $needle, bool $beforeNeedle = false): string|false {}

function grapheme_stristr(string $haystack, string $needle, bool $beforeNeedle = false): string|false {}

/** @param int $next */
function grapheme_extract(string $haystack, int $size, int $type = GRAPHEME_EXTR_COUNT, int $offset = 0, &$next = null): string|false {}

/* idn */

/** @param array $idna_info */
function idn_to_ascii(string $domain, int $flags = IDNA_DEFAULT, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}

/** @param array $idna_info */
function idn_to_utf8(string $domain, int $flags = IDNA_DEFAULT, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}

/* locale */


function locale_get_default(): string {}

function locale_set_default(string $locale): bool {}

function locale_get_primary_language(string $locale): ?string {}

function locale_get_script(string $locale): ?string {}

function locale_get_region(string $locale): ?string {}

/**
 * @return array<string, string>|false|null
 * @refcount 1
 */
function locale_get_keywords(string $locale): array|false|null {}

function locale_get_display_script(string $locale, ?string $displayLocale = null): string|false {}

function locale_get_display_region(string $locale, ?string $displayLocale = null): string|false {}

function locale_get_display_name(string $locale, ?string $displayLocale = null): string|false {}

function locale_get_display_language(string $locale, ?string $displayLocale = null): string|false {}

function locale_get_display_variant(string $locale, ?string $displayLocale = null): string|false {}

function locale_compose(array $subtags): string|false {}

function locale_parse(string $locale): ?array {}

function locale_get_all_variants(string $locale): ?array {}

function locale_filter_matches(string $languageTag, string $locale, bool $canonicalize = false): ?bool {}

function locale_canonicalize(string $locale): ?string {}

function locale_lookup(array $languageTag, string $locale, bool $canonicalize = false, ?string $defaultLocale = null): ?string {}

function locale_accept_from_http(string $header): string|false {}

/* msgformat */

function msgfmt_create(string $locale, string $pattern): ?MessageFormatter {}

function msgfmt_format(MessageFormatter $formatter, array $values): string|false {}

function msgfmt_format_message(string $locale, string $pattern, array $values): string|false {}

/**
 * @return array<int, int|float|string>|false
 * @refcount 1
 */
function msgfmt_parse(MessageFormatter $formatter, string $string): array|false {}

/**
 * @return array<int, int|float|string>|false
 * @refcount 1
 */
function msgfmt_parse_message(string $locale, string $pattern, string $message): array|false {}

function msgfmt_set_pattern(MessageFormatter $formatter, string $pattern): bool {}

function msgfmt_get_pattern(MessageFormatter $formatter): string|false {}

function msgfmt_get_locale(MessageFormatter $formatter): string {}

function msgfmt_get_error_code(MessageFormatter $formatter): int {}

function msgfmt_get_error_message(MessageFormatter $formatter): string {}

/* normalizer */

function normalizer_normalize(string $string, int $form = Normalizer::FORM_C): string|false {}

function normalizer_is_normalized(string $string, int $form = Normalizer::FORM_C): bool {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
function normalizer_get_raw_decomposition(string $string, int $form = Normalizer::FORM_C): ?string {}
#endif

/* resourceboundle */

function resourcebundle_create(?string $locale, ?string $bundle, bool $fallback = true): ?ResourceBundle {}

function resourcebundle_get(ResourceBundle $bundle, string|int $index, bool $fallback = true): ResourceBundle|array|string|int|null {}

function resourcebundle_count(ResourceBundle $bundle): int {}

/**
 * @return array<int, string>|false
 * @refcount 1
 */
function resourcebundle_locales(string $bundle): array|false {}

function resourcebundle_get_error_code(ResourceBundle $bundle): int {}

function resourcebundle_get_error_message(ResourceBundle $bundle): string {}

/* timezone */

function intltz_count_equivalent_ids(string $timezoneId): int|false {}

function intltz_create_default(): IntlTimeZone {}

/** @param IntlTimeZone|string|int|float|null $countryOrRawOffset */
function intltz_create_enumeration($countryOrRawOffset = null): IntlIterator|false {}

function intltz_create_time_zone(string $timezoneId): ?IntlTimeZone {}

function intltz_create_time_zone_id_enumeration(int $type, ?string $region = null, ?int $rawOffset = null): IntlIterator|false {}

function intltz_from_date_time_zone(DateTimeZone $timezone): ?IntlTimeZone {}

/** @param bool $isSystemId */
function intltz_get_canonical_id(string $timezoneId, &$isSystemId = null): string|false {}

function intltz_get_display_name(IntlTimeZone $timezone, bool $dst = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null): string|false {}

function intltz_get_dst_savings(IntlTimeZone $timezone): int {}

function intltz_get_equivalent_id(string $timezoneId, int $offset): string|false {}

function intltz_get_error_code(IntlTimeZone $timezone): int|false {}

function intltz_get_error_message(IntlTimeZone $timezone): string|false {}

function intltz_get_gmt(): IntlTimeZone {}

function intltz_get_id(IntlTimeZone $timezone): string|false {}

/**
 * @param int $rawOffset
 * @param int $dstOffset
 */
function intltz_get_offset(IntlTimeZone $timezone, float $timestamp, bool $local, &$rawOffset, &$dstOffset): bool {}

function intltz_get_raw_offset(IntlTimeZone $timezone): int {}

function intltz_get_region(string $timezoneId): string|false {}

function intltz_get_tz_data_version(): string|false {}

function intltz_get_unknown(): IntlTimeZone {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
function intltz_get_windows_id(string $timezoneId): string|false {}

function intltz_get_id_for_windows_id(string $timezoneId, ?string $region = null): string|false {}
#endif

function intltz_has_same_rules(IntlTimeZone $timezone, IntlTimeZone $other): bool {}

function intltz_to_date_time_zone(IntlTimeZone $timezone): DateTimeZone|false {}

function intltz_use_daylight_time(IntlTimeZone $timezone): bool {}

#if U_ICU_VERSION_MAJOR_NUM >= 74
function intltz_get_iana_id(string $timezoneId): string|false {}
#endif

/* transliterator */

function transliterator_create(string $id, int $direction = Transliterator::FORWARD): ?Transliterator {}

function transliterator_create_from_rules(string $rules, int $direction = Transliterator::FORWARD): ?Transliterator {}

/**
 * @return array<int, string>|false
 * @refcount 1
 */
function transliterator_list_ids(): array|false {}

function transliterator_create_inverse(Transliterator $transliterator): ?Transliterator {}

function transliterator_transliterate(Transliterator|string $transliterator, string $string, int $start = 0, int $end = -1): string|false {}

function transliterator_get_error_code(Transliterator $transliterator): int|false {}

function transliterator_get_error_message(Transliterator $transliterator): string|false {}
