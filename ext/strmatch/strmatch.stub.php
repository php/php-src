<?php

/** @generate-class-entries */

namespace Str;

/**
 * @strict-properties
 * @not-serializable
 */
final class Matcher
{
    public function __construct(array $keywords) {}

    public function matches(string $subject): bool {}

    public function countMatches(string $subject): int {}

    /** @return list<string> */
    public function getMatchedKeywords(string $subject): array {}

    /** @return list<\Str\MatchResult> */
    public function matchAll(string $subject): array {}

    /** @return list<string> */
    public function getKeywords(): array {}
}

/** @strict-properties */
final class MatchResult
{
    private function __construct() {}

    public readonly int $offset;
    public readonly int $length;
    public readonly string $keyword;
}

/**
 * @strict-properties
 * @not-serializable
 */
final class Replacer
{
    public function __construct(array $replacements) {}

    /** @param int $count */
    public function replace(string $subject, &$count = null): string {}

    /** @return array<string|int, string> */
    public function getReplacements(): array {}
}
