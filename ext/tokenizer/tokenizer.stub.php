<?php

function token_get_all(string $source, int $flags = 0): array {}

function token_name(int $token): string {}

class PhpToken {
    /** @return PhpToken[] */
    public static function getAll(string $code, int $flags = 0): array;

    /** @param int|string|array $kind */
    public function is($kind): bool;

    public function isIgnorable(): bool;

    public function getTokenName(): string;
}
