<?php

class Transliterator
{
    final private function __construct() {}

    /** @return Transliterator|null */
    public static function create(string $id, int $direction = Transliterator::FORWARD) {}

    /** @return Transliterator|null */
    public static function createFromRules(string $rules, int $direction = Transliterator::FORWARD) {}

    /** @return Transliterator|null */
    public function createInverse() {}

    /** @return array|false */
    public static function listIDs() {}

    /** @return string|false */
    public function transliterate(string $subject, int $start = 0, int $end = -1) {}

    /** @return int|false */
    public function getErrorCode() {}

    /** @return string|false */
    public function getErrorMessage() {}
}
