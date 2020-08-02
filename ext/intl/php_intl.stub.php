<?php

/** @generate-function-entries */

/* calendar */

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlcal_create_instance($timeZone = null, ?string $locale = null): IntlCalendar|null {}

function intlcal_get_keyword_values_for_locale(string $key, string $locale, bool $commonlyUsed): IntlIterator|false {}

function intlcal_get_now(): float {}

function intlcal_get_available_locales(): array {}

function intlcal_get(IntlCalendar $calendar, int $field): int {}

function intlcal_get_time(IntlCalendar $calendar): float {}

function intlcal_set_time(IntlCalendar $calendar, float $date): bool {}

function intlcal_add(IntlCalendar $calendar, int $field, int $amount): bool {}

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlcal_set_time_zone(IntlCalendar $calendar, $timeZone): bool {}

function intlcal_after(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_before(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_set(IntlCalendar $calendar, int $year, int $month, int $dayOfMonth = UNKNOWN, int $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN): bool {}

/** @param int|bool $amountOrUpOrDown */
function intlcal_roll(IntlCalendar $calendar, int $field, $amountOrUpOrDown): bool {}

function intlcal_clear(IntlCalendar $calendar, ?int $field = null): bool {}

function intlcal_field_difference(IntlCalendar $calendar, float $when, int $field): int {}

function intlcal_get_actual_maximum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_actual_minimum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_day_of_week_type(IntlCalendar $calendar, int $dayOfWeek): int {}

function intlcal_get_first_day_of_week(IntlCalendar $calendar): int {}

function intlcal_get_least_maximum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_greatest_minimum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_locale(IntlCalendar $calendar, int $localeType): string {}

function intlcal_get_maximum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_minimal_days_in_first_week(IntlCalendar $calendar): int {}

function intlcal_set_minimal_days_in_first_week(IntlCalendar $calendar, int $numberOfDays): bool {}

function intlcal_get_minimum(IntlCalendar $calendar, int $field): int {}

function intlcal_get_time_zone(IntlCalendar $calendar): IntlTimeZone|false {}

function intlcal_get_type(IntlCalendar $calendar): string {}

function intlcal_get_weekend_transition(IntlCalendar $calendar, int $dayOfWeek): int {}

function intlcal_in_daylight_time(IntlCalendar $calendar): bool {}

function intlcal_is_lenient(IntlCalendar $calendar): bool {}

function intlcal_is_set(IntlCalendar $calendar, int $field): bool {}

function intlcal_is_equivalent_to(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_is_weekend(IntlCalendar $calendar, ?float $date = null): bool {}

function intlcal_set_first_day_of_week(IntlCalendar $calendar, int $dayOfWeek): bool {}

function intlcal_set_lenient(IntlCalendar $calendar, bool $isLenient): bool {}

function intlcal_get_repeated_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_equals(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_get_skipped_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_set_repeated_wall_time_option(IntlCalendar $calendar, int $wallTimeOption): bool {}

function intlcal_set_skipped_wall_time_option(IntlCalendar $calendar, int $wallTimeOption): bool {}

/** @param DateTime|string $dateTime */
function intlcal_from_date_time($dateTime, ?string $locale = null): ?IntlCalendar {}

function intlcal_to_date_time(IntlCalendar $calendar): DateTime|false {}

function intlcal_get_error_code(IntlCalendar $calendar): int|false {}

function intlcal_get_error_message(IntlCalendar $calendar): string|false {}

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlgregcal_create_instance($timeZoneOrYear = UNKNOWN, $localeOrMonth = UNKNOWN, $dayOfMonth = UNKNOWN, $hour = UNKNOWN, $minute = UNKNOWN, $second = UNKNOWN): ?IntlGregorianCalendar {}

function intlgregcal_set_gregorian_change(IntlGregorianCalendar $calendar, float $change): bool {}

function intlgregcal_get_gregorian_change(IntlGregorianCalendar $calendar): float {}

function intlgregcal_is_leap_year(IntlGregorianCalendar $calendar, int $year): bool {}

/* collator */

function collator_create(string $locale): ?Collator {}

function collator_compare(Collator $object, string $str1, string $str2): int|false {}

function collator_get_attribute(Collator $object, int $attr): int|false {}

function collator_set_attribute(Collator $object, int $attr, int $val): bool {}

function collator_get_strength(Collator $object): int {}

function collator_set_strength(Collator $object, int $strength): bool {}

function collator_sort(Collator $object, array &$arr, int $sort_flag = Collator::SORT_REGULAR): bool {}

function collator_sort_with_sort_keys(Collator $object, array &$arr): bool {}

function collator_asort(Collator $object, array &$arr, int $sort_flag = Collator::SORT_REGULAR): bool {}

function collator_get_locale(Collator $object, int $type): string|false {}

function collator_get_error_code(Collator $object): int|false {}

function collator_get_error_message(Collator $object): string|false {}

function collator_get_sort_key(Collator $object, string $str): string|false {}

/* common */

function intl_get_error_code(): int {}

function intl_get_error_message(): string {}

function intl_is_failure(int $error_code): bool {}

function intl_error_name(int $error_code): string {}

/* dateformat */

/**
 * @param IntlTimeZone|DateTimeZone|string|null $timezone
 * @param IntlCalendar|int|null $calendar
 */
function datefmt_create(?string $locale, int $datetype, int $timetype, $timezone = null, $calendar = null, string $pattern = ""): ?IntlDateFormatter {}

function datefmt_get_datetype(IntlDateFormatter $df): int|false {}

function datefmt_get_timetype(IntlDateFormatter $df): int|false {}

function datefmt_get_calendar(IntlDateFormatter $df): int|false {}

/** @param IntlCalendar|int|null $which */
function datefmt_set_calendar(IntlDateFormatter $df, $which): bool {}

function datefmt_get_timezone_id(IntlDateFormatter $df): string|false {}

function datefmt_get_calendar_object(IntlDateFormatter $df): IntlCalendar|false|null {}

function datefmt_get_timezone(IntlDateFormatter $df): IntlTimeZone|false {}

/** @param IntlTimeZone|DateTimeZone|string|null $zone */
function datefmt_set_timezone(IntlDateFormatter $df, $zone): ?bool {}

function datefmt_set_pattern(IntlDateFormatter $df, string $pattern): bool {}

function datefmt_get_pattern(IntlDateFormatter $df): string|false {}

function datefmt_get_locale(IntlDateFormatter $df, int $which = ULOC_ACTUAL_LOCALE): string|false {}

function datefmt_set_lenient(IntlDateFormatter $df, bool $lenient): void {}

function datefmt_is_lenient(IntlDateFormatter $df): bool {}

/** @param mixed $value */
function datefmt_format(IntlDateFormatter $df, $value): string|false {}

/**
 * @param IntlCalendar|DateTimeInterface $object
 * @param array|int|string|null $format
 */
function datefmt_format_object($object, $format = null, ?string $locale = null): string|false {}

/** @param int $position */
function datefmt_parse(IntlDateFormatter $df, string $value, &$position = null): int|float|false {}

/** @param int $position */
function datefmt_localtime(IntlDateFormatter $df, string $value, &$position = null): array|false {}

function datefmt_get_error_code(IntlDateFormatter $df): int {}

function datefmt_get_error_message(IntlDateFormatter $df): string {}

/* formatter */

function numfmt_create(string $locale, int $style, string $pattern = ""): ?NumberFormatter {}

function numfmt_format(NumberFormatter $fmt, int|float $value, int $type = NumberFormatter::TYPE_DEFAULT): string|false {}

/** @param int $position */
function numfmt_parse(NumberFormatter $fmt, string $value, int $type = NumberFormatter::TYPE_DOUBLE, &$position = null): int|float|false {}

function numfmt_format_currency(NumberFormatter $fmt, float $value, string $currency): string|false {}

/**
 * @param string $currency
 * @param int $position
 */
function numfmt_parse_currency(NumberFormatter $fmt, string $value, &$currency, &$position = null): float|false {}

/** @param int|float $value */
function numfmt_set_attribute(NumberFormatter $fmt, int $attr, $value): bool {}

function numfmt_get_attribute(NumberFormatter $fmt, int $attr): int|double|false {}

function numfmt_set_text_attribute(NumberFormatter $fmt, int $attr, string $value): bool {}

function numfmt_get_text_attribute(NumberFormatter $fmt, int $attr): string|false {}

function numfmt_set_symbol(NumberFormatter $fmt, int $attr, string $value): bool {}

function numfmt_get_symbol(NumberFormatter $fmt, int $attr): string|false {}

function numfmt_set_pattern(NumberFormatter $fmt, string $pattern): bool {}

function numfmt_get_pattern(NumberFormatter $fmt): string|false {}

function numfmt_get_locale(NumberFormatter $fmt, int $type = ULOC_ACTUAL_LOCALE): string|false {}

function numfmt_get_error_code(NumberFormatter $fmt): int {}

function numfmt_get_error_message(NumberFormatter $fmt): string {}

/* grapheme */

function grapheme_strlen(string $input): int|false|null {}

function grapheme_strpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_stripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strrpos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_strripos(string $haystack, string $needle, int $offset = 0): int|false {}

function grapheme_substr(string $string, int $start, ?int $length = null): string|false {}

function grapheme_strstr(string $haystack, string $needle, bool $before_needle = false): string|false {}

function grapheme_stristr(string $haystack, string $needle, bool $before_needle = false): string|false {}

/** @param int $next */
function grapheme_extract(string $haystack, int $size, int $extract_type = GRAPHEME_EXTR_COUNT, int $start = 0, &$next = null): string|false {}

/* idn */

/** @param array $idna_info */
function idn_to_ascii(string $domain, int $options = 0, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}

/** @param array $idna_info */
function idn_to_utf8(string $domain, int $options = 0, int $variant = INTL_IDNA_VARIANT_UTS46, &$idna_info = null): string|false {}

/* locale */


function locale_get_default(): string {}

function locale_set_default(string $name): bool {}

function locale_get_primary_language(string $locale): ?string {}

function locale_get_script(string $locale): ?string {}

function locale_get_region(string $locale): ?string {}

function locale_get_keywords(string $locale): array|false|null {}

function locale_get_display_script(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_region(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_name(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_language(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_variant(string $locale, ?string $in_locale = null): string|false {}

function locale_compose(array $subtags): string|false {}

function locale_parse(string $locale): ?array {}

function locale_get_all_variants(string $locale): ?array {}

function locale_filter_matches(string $langtag, string $locale, bool $canonicalize = false): ?bool {}

function locale_canonicalize(string $locale): ?string {}

function locale_lookup(array $langtag, string $locale, bool $canonicalize = false, ?string $def = null): ?string {}

function locale_accept_from_http(string $header): string|false {}

/* msgformat */

function msgfmt_create(string $locale, string $pattern): ?MessageFormatter {}

function msgfmt_format(MessageFormatter $fmt, array $args): string|false {}

function msgfmt_format_message(string $locale, string $pattern, array $args): string|false {}

function msgfmt_parse(MessageFormatter $fmt, string $value): array|false {}

function msgfmt_parse_message(string $locale, string $pattern, string $source): array|false {}

function msgfmt_set_pattern(MessageFormatter $fmt, string $pattern): bool {}

function msgfmt_get_pattern(MessageFormatter $fmt): string|false {}

function msgfmt_get_locale(MessageFormatter $fmt): string {}

function msgfmt_get_error_code(MessageFormatter $fmt): int {}

function msgfmt_get_error_message(MessageFormatter $fmt): string {}

/* normalizer */

function normalizer_normalize(string $input, int $form = Normalizer::FORM_C): string|false {}

function normalizer_is_normalized(string $input, int $form = Normalizer::FORM_C): bool {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
function normalizer_get_raw_decomposition(string $input, int $form = Normalizer::FORM_C): ?string {}
#endif

/* resourceboundle */

function resourcebundle_create(?string $locale, ?string $bundlename, bool $fallback = true): ?ResourceBundle {}

/**
 * @param string|int $index
 * @return mixed
 */
function resourcebundle_get(ResourceBundle $bundle, $index) {}

function resourcebundle_count(ResourceBundle $bundle): int {}

function resourcebundle_locales(string $bundlename): array|false {}

function resourcebundle_get_error_code(ResourceBundle $bundle): int {}

function resourcebundle_get_error_message(ResourceBundle $bundle): string {}

/* timezone */

function intltz_count_equivalent_ids(string $zoneId): int|false {}

function intltz_create_default(): IntlTimeZone {}

/**
 * @param IntlTimeZone|string|int|double|null $countryOrRawOffset
 * @return IntlIterator|false
 */
function intltz_create_enumeration($countryOrRawOffset = null) {}

function intltz_create_time_zone(string $zoneId): ?IntlTimeZone {}

function intltz_create_time_zone_id_enumeration(int $zoneType, ?string $region = null, ?int $rawOffset = null): IntlIterator|false {}

function intltz_from_date_time_zone(DateTimeZone $zone): ?IntlTimeZone {}

/** @param bool $isSystemID */
function intltz_get_canonical_id(string $zoneId, &$isSystemID = null): string|false {}

function intltz_get_display_name(IntlTimeZone $tz, bool $isDaylight = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null): string|false {}

function intltz_get_dst_savings(IntlTimeZone $tz): int {}

function intltz_get_equivalent_id(string $zoneId, int $index): string|false {}

function intltz_get_error_code(IntlTimeZone $tz): int|false {}

function intltz_get_error_message(IntlTimeZone $tz): string|false {}

function intltz_get_gmt(): IntlTimeZone {}

function intltz_get_id(IntlTimeZone $tz): string|false {}

function intltz_get_offset(IntlTimeZone $tz, float $date, bool $local, &$rawOffset, &$dstOffset): bool {}

function intltz_get_raw_offset(IntlTimeZone $tz): int {}

function intltz_get_region(string $zoneId): string|false {}

function intltz_get_tz_data_version(): string|false {}

function intltz_get_unknown(): IntlTimeZone {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
function intltz_get_windows_id(string $timezone): string|false {}

function intltz_get_id_for_windows_id(string $timezone, ?string $region = null): string|false {}
#endif

function intltz_has_same_rules(IntlTimeZone $tz, IntlTimeZone $otherTimeZone): bool {}

function intltz_to_date_time_zone(IntlTimeZone $tz): DateTimeZone|false {}

function intltz_use_daylight_time(IntlTimeZone $tz): bool {}

/* transliterator */

function transliterator_create(string $id, int $direction = Transliterator::FORWARD): ?Transliterator {}

function transliterator_create_from_rules(string $rules, int $direction = Transliterator::FORWARD): ?Transliterator {}

function transliterator_list_ids(): array|false {}

function transliterator_create_inverse(Transliterator $orig_trans): ?Transliterator {}

/** @param Transliterator|string $transliterator */
function transliterator_transliterate($transliterator, string $subject, int $start = 0, int $end = -1): string|false {}

function transliterator_get_error_code(Transliterator $trans): int|false {}

function transliterator_get_error_message(Transliterator $trans): string|false {}
