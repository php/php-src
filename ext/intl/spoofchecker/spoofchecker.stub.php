<?php

/** @generate-function-entries */

class Spoofchecker
{
    public function __construct() {}

    /**
     * @param int $error
     * @return bool
     */
    public function isSuspicious(string $text, &$error = null) {}

    /**
     * @param int $error
     * @return bool
     */
    public function areConfusable(string $s1, string $s2, &$error = null) {}

    /** @return void */
    public function setAllowedLocales(string $locale_list) {}

    /** @return void */
    public function setChecks(int $checks) {}

#if U_ICU_VERSION_MAJOR_NUM >= 58
    /** @return void */
    public function setRestrictionLevel(int $level) {}
#endif
}
