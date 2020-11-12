<?php

/** @generate-function-entries */

class Spoofchecker
{
    public function __construct() {}

    /**
     * @param int $errorCode
     * @return bool
     */
    public function isSuspicious(string $string, &$errorCode = null) {}

    /**
     * @param int $errorCode
     * @return bool
     */
    public function areConfusable(string $string1, string $string2, &$errorCode = null) {}

    /** @return void */
    public function setAllowedLocales(string $locales) {}

    /** @return void */
    public function setChecks(int $checks) {}

#if U_ICU_VERSION_MAJOR_NUM >= 58
    /** @return void */
    public function setRestrictionLevel(int $level) {}
#endif
}
