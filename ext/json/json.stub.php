<?php

/** @generate-class-entries */

function json_encode(mixed $value, int $flags = 0, int $depth = 512): string|false {}

function json_decode(string $json, ?bool $associative = null, int $depth = 512, int $flags = 0): mixed {}

function json_last_error(): int {}

function json_last_error_msg(): string {}

interface JsonSerializable
{
    /** @tentative-return-type */
    public function jsonSerialize(): mixed;
}

class JsonException extends Exception
{
}
