<?php

/** @generate-function-entries */

function json_encode(mixed $value, int $options = 0, int $depth = 512): string|false {}

function json_decode(string $json, ?bool $assoc = null, int $depth = 512, int $options = 0): mixed {}

function json_last_error(): int {}

function json_last_error_msg(): string {}

interface JsonSerializable
{
    /** @return mixed */
    public function jsonSerialize();
}
