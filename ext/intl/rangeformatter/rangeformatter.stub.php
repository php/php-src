<?php

/** @generate-class-entries */

/**
 * @not-serializable
 * @strict-properties
 */
final class IntlNumberRangeFormatter {

    /** @cvalue UNUM_RANGE_COLLAPSE_AUTO */
    public const int COLLAPSE_AUTO = UNKNOWN;

    /** @cvalue UNUM_RANGE_COLLAPSE_NONE */
    public const int COLLAPSE_NONE = UNKNOWN;

    /** @cvalue UNUM_RANGE_COLLAPSE_UNIT */
    public const int COLLAPSE_UNIT = UNKNOWN;

    /** @cvalue UNUM_RANGE_COLLAPSE_ALL */
    public const int COLLAPSE_ALL = UNKNOWN;

    /** @cvalue UNUM_IDENTITY_FALLBACK_SINGLE_VALUE */
    public const int IDENTITY_FALLBACK_SINGLE_VALUE = UNKNOWN;

    /** @cvalue UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE */
    public const int IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE = UNKNOWN;

    /** @cvalue UNUM_IDENTITY_FALLBACK_APPROXIMATELY */
    public const int IDENTITY_FALLBACK_APPROXIMATELY = UNKNOWN;

    /** @cvalue UNUM_IDENTITY_FALLBACK_RANGE */
    public const int IDENTITY_FALLBACK_RANGE = UNKNOWN;

    private function __construct() {}

    public static function createFromSkeleton(string $skeleton, string $locale, int $collapse, int $identityFallback): IntlNumberRangeFormatter {}

    public function format(float|int $start, float|int $end): string {}
}
