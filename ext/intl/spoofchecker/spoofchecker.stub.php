<?php

/** @generate-class-entries */

/** @not-serializable */
class Spoofchecker
{
    /**
     * @var int
     * @cvalue USPOOF_SINGLE_SCRIPT_CONFUSABLE
     */
    public const SINGLE_SCRIPT_CONFUSABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_MIXED_SCRIPT_CONFUSABLE
     */
    public const MIXED_SCRIPT_CONFUSABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_WHOLE_SCRIPT_CONFUSABLE
     */
    public const WHOLE_SCRIPT_CONFUSABLE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_ANY_CASE
     */
    public const ANY_CASE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_SINGLE_SCRIPT
     */
    public const SINGLE_SCRIPT = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_INVISIBLE
     */
    public const INVISIBLE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_CHAR_LIMIT
     */
    public const CHAR_LIMIT = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 58
    /**
     * @var int
     * @cvalue USPOOF_ASCII
     */
    public const ASCII = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_HIGHLY_RESTRICTIVE
     */
    public const HIGHLY_RESTRICTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_MODERATELY_RESTRICTIVE
     */
    public const MODERATELY_RESTRICTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_MINIMALLY_RESTRICTIVE
     */
    public const MINIMALLY_RESTRICTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_UNRESTRICTIVE
     */
    public const UNRESTRICTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_SINGLE_SCRIPT_RESTRICTIVE
     */
    public const SINGLE_SCRIPT_RESTRICTIVE = UNKNOWN;
    /**
     * @var int
     * @cvalue USPOOF_MIXED_NUMBERS
     */
    public const MIXED_NUMBERS = UNKNOWN;
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 62
    /**
     * @var int
     * @cvalue USPOOF_HIDDEN_OVERLAY
     */
    public const HIDDEN_OVERLAY = UNKNOWN;
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
}
