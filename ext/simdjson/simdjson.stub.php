<?php

/** @generate-function-entries */

final class JsonParser
{
    public static function parse(string $json, bool $associative = false, int $depth = 512, int $flags = 0): mixed {}

    public static function isValid(string $json): bool {}

    public static function getKeyValue(string $json, string $key, bool $associative = false, int $depth = 512, int $flags = 0): mixed {}

    public static function getKeyCount(string $json, string $key, int $depth = 512): int {}

    public static function keyExists(string $json, string $key, int $depth = 512): ?bool {}
}

final class JsonEncoder
{
    public static function encode(mixed $value, int $flags = 0, int $depth = 512): string {}
}
