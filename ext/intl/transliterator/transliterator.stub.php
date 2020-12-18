<?php

/** @generate-function-entries */

class Transliterator
{
    final private function __construct() {}

    /**
     * @return Transliterator|null
     * @alias transliterator_create
     */
    public static function create(string $id, int $direction = Transliterator::FORWARD) {}

    /**
     * @return Transliterator|null
     * @alias transliterator_create_from_rules
     */
    public static function createFromRules(string $rules, int $direction = Transliterator::FORWARD) {}

    /**
     * @return Transliterator|null
     * @alias transliterator_create_inverse
     */
    public function createInverse() {}

    /**
     * @return array|false
     * @alias transliterator_list_ids
     */
    public static function listIDs() {}

    /**
     * @return string|false
     * @alias transliterator_transliterate
     */
    public function transliterate(string $string, int $start = 0, int $end = -1) {}

    /**
     * @return int|false
     * @alias transliterator_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string|false
     * @alias transliterator_get_error_message
     */
    public function getErrorMessage() {}
}
