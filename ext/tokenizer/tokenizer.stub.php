<?php

function token_get_all(string $source, int $flags = 0): array {}

function token_name(int $token): string {}

class PhpToken {
    /** @param int|string|array $kind */
    public function is($kind): bool;

    public function isIgnorable(): bool;

    public function getTokenName(): string;
}
