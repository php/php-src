<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue TOKEN_PARSE
 */
const TOKEN_PARSE = UNKNOWN;

function token_get_all(string $code, int $flags = 0): array {}

function token_name(int $id): string {}

class PhpToken implements Stringable
{
    public int $id;
    public string $text;
    public int $line;
    public int $pos;

    /** @return static[] */
    public static function tokenize(string $code, int $flags = 0): array {}

    final public function __construct(int $id, string $text, int $line = -1, int $pos = -1) {}

    /** @param int|string|array $kind */
    public function is($kind): bool {}

    public function isIgnorable(): bool {}

    public function getTokenName(): ?string {}

    public function __toString(): string {}
}
