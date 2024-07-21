<?php

/** @generate-class-entries */

/** @not-serializable */
class Spoofchecker
{
    /** @cvalue USPOOF_SINGLE_SCRIPT_CONFUSABLE */
    public const int SINGLE_SCRIPT_CONFUSABLE = UNKNOWN;
    /** @cvalue USPOOF_MIXED_SCRIPT_CONFUSABLE */
    public const int MIXED_SCRIPT_CONFUSABLE = UNKNOWN;
    /** @cvalue USPOOF_WHOLE_SCRIPT_CONFUSABLE */
    public const int WHOLE_SCRIPT_CONFUSABLE = UNKNOWN;
    /** @cvalue USPOOF_ANY_CASE */
    public const int ANY_CASE = UNKNOWN;
    /** @cvalue USPOOF_SINGLE_SCRIPT */
    public const int SINGLE_SCRIPT = UNKNOWN;
    /** @cvalue USPOOF_INVISIBLE */
    public const int INVISIBLE = UNKNOWN;
    /** @cvalue USPOOF_CHAR_LIMIT */
    public const int CHAR_LIMIT = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 58
    /** @cvalue USPOOF_ASCII */
    public const int ASCII = UNKNOWN;
    /** @cvalue USPOOF_HIGHLY_RESTRICTIVE */
    public const int HIGHLY_RESTRICTIVE = UNKNOWN;
    /** @cvalue USPOOF_MODERATELY_RESTRICTIVE */
    public const int MODERATELY_RESTRICTIVE = UNKNOWN;
    /** @cvalue USPOOF_MINIMALLY_RESTRICTIVE */
    public const int MINIMALLY_RESTRICTIVE = UNKNOWN;
    /** @cvalue USPOOF_UNRESTRICTIVE */
    public const int UNRESTRICTIVE = UNKNOWN;
    /** @cvalue USPOOF_SINGLE_SCRIPT_RESTRICTIVE */
    public const int SINGLE_SCRIPT_RESTRICTIVE = UNKNOWN;
    /** @cvalue USPOOF_MIXED_NUMBERS */
    public const int MIXED_NUMBERS = UNKNOWN;
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 62
    /** @cvalue USPOOF_HIDDEN_OVERLAY */
    public const int HIDDEN_OVERLAY = UNKNOWN;
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 74
    /** @cvalue UBIDI_LTR */
    public const int UBIDI_LTR = UNKNOWN;
    /** @cvalue UBIDI_RTL */
    public const int UBIDI_RTL = UNKNOWN;
    /** @cvalue UBIDI_MIXED */
    public const int UBIDI_MIXED = UNKNOWN;
    /** @cvalue UBIDI_NEUTRAL */
    public const int UBIDI_NEUTRAL = UNKNOWN;
#endif

    public function __construct() {}

    /**
     * @param int $errorCode
     * @tentative-return-type
     */
    public function isSuspicious(string $string, &$errorCode = null): bool {}

    /**
     * @param int $errorCode
     * @tentative-return-type
     */
    public function areConfusable(string $string1, string $string2, &$errorCode = null): bool {}

    /** @tentative-return-type */
    public function setAllowedLocales(string $locales): void {}

    /** @tentative-return-type */
    public function setChecks(int $checks): void {}

#if U_ICU_VERSION_MAJOR_NUM >= 58
    /** @tentative-return-type */
    public function setRestrictionLevel(int $level): void {}
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 74
    /**
     * @param int $errorCode
     */
    public function areBidiConfusable(int $direction, string $string1, string $string2, &$errorCode = null): bool {}
#endif
}
