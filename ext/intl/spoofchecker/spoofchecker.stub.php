<?php

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

    /** @return void */
    public function setRestrictionLevel(int $level) {}
}
