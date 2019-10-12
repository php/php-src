<?php

class Spoofchecker
{
    public function __construct() {}

    /** @return bool */
    public function isSuspicious(string $text, &$error = null) {}

    /** @return bool */
    public function areConfusable(string $s1, string $s2, &$error = null) {}

    /** @return null|false */
    public function setAllowedLocales(string $locale_list) {}

    /** @return null|false */
    public function setChecks(int $checks) {}

#if U_ICU_VERSION_MAJOR_NUM >= 58
    /** @return null|false */
    public function setRestrictionLevel(int $level) {}
#endif
}
