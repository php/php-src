<?php

/** @generate-function-entries */

class Spoofchecker
{
    public function __construct() {}

    /** @return bool */
    public function isSuspicious(string $text, &$error = null) {}

    /** @return bool */
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
