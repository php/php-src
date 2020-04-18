<?php

/** @generate-function-entries */

function token_get_all(string $source, int $flags = 0): array {}

function token_name(int $token): string {}

class PhpToken implements Stringable
{
    /** @return static[] */
    public static function getAll(string $code, int $flags = 0): array {}

    public final function __construct(int $id, string $text, int $line = -1, int $pos = -1) {}

    /** @param int|string|array $kind */
    public function is($kind): bool {}

    public function isIgnorable(): bool {}

    public function getTokenName(): ?string {}

    public function __toString(): string {}
}
