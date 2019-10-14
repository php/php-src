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

function transliterator_create(string $id, int $direction = Transliterator::FORWARD): ?Transliterator {}

function transliterator_create_from_rules(string $rules, int $direction = Transliterator::FORWARD): ?Transliterator {}

function transliterator_list_ids(): array|false {}

function transliterator_create_inverse(Transliterator $orig_trans): ?Transliterator {}

/** @param Transliterator|string */
function transliterator_transliterate($transliterator, string $subject, int $start = 0, int $end = -1): string|false {}

function transliterator_get_error_code(Transliterator $trans): int|false {}

function transliterator_get_error_message(Transliterator $trans): string|false {}
