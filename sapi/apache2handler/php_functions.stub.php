<?php

/** @generate-class-entries */

function apache_lookup_uri(string $filename): object|false {}

function virtual(string $uri): bool {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function apache_request_headers(): array {}

/**
 * @return array<string, string>
 * @alias apache_request_headers
 */
function getallheaders(): array {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function apache_response_headers(): array {}

function apache_note(string $note_name, ?string $note_value = null): string|false {}

function apache_setenv(string $variable, string $value, bool $walk_to_top = false): bool {}

function apache_getenv(string $variable, bool $walk_to_top = false): string|false {}

function apache_get_version(): string|false {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function apache_get_modules(): array {}
