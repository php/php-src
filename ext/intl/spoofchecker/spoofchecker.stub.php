<?php

/** @generate-class-entries */

class Spoofchecker
{
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
