<?php

/** @generate-class-entries */

class Transliterator
{
    public string $id;

    final private function __construct() {}

    /**
     * @tentative-return-type
     * @alias transliterator_create
     */
    public static function create(string $id, int $direction = Transliterator::FORWARD): ?Transliterator {}

    /**
     * @tentative-return-type
     * @alias transliterator_create_from_rules
     */
    public static function createFromRules(string $rules, int $direction = Transliterator::FORWARD): ?Transliterator {}

    /**
     * @tentative-return-type
     * @alias transliterator_create_inverse
     */
    public function createInverse(): ?Transliterator {}

    /**
     * @tentative-return-type
     * @alias transliterator_list_ids
     */
    public static function listIDs(): array|false {}

    /**
     * @tentative-return-type
     * @alias transliterator_transliterate
     */
    public function transliterate(string $string, int $start = 0, int $end = -1): string|false {}

    /**
     * @tentative-return-type
     * @alias transliterator_get_error_code
     */
    public function getErrorCode(): int|false {}

    /**
     * @tentative-return-type
     * @alias transliterator_get_error_message
     */
    public function getErrorMessage(): string|false {}
}
