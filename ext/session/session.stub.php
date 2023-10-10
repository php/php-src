<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue php_session_disabled
 */
const PHP_SESSION_DISABLED = 0;
/**
 * @var int
 * @cvalue php_session_none
 */
const PHP_SESSION_NONE = 1;
/**
 * @var int
 * @cvalue php_session_active
 */
const PHP_SESSION_ACTIVE = 2;

/** @refcount 1 */
function session_name(?string $name = null): string|false {}

/** @refcount 1 */
function session_module_name(?string $module = null): string|false {}

/** @refcount 1 */
function session_save_path(?string $path = null): string|false {}

function session_id(?string $id = null): string|false {}

/** @refcount 1 */
function session_create_id(string $prefix = ""): string|false {}

function session_regenerate_id(bool $delete_old_session = false): bool {}

function session_decode(string $data): bool {}

/** @refcount 1 */
function session_encode(): string|false {}

function session_destroy(): bool {}

function session_unset(): bool {}

function session_gc(): int|false {}

/**
 * @return array<string, mixed>
 * @refcount 1
 */
function session_get_cookie_params(): array {}

function session_write_close(): bool {}

function session_abort(): bool {}

function session_reset(): bool {}

function session_status(): int {}

function session_register_shutdown(): void {}

/** @alias session_write_close */
function session_commit(): bool {}

/**
 * @param callable|object $open
 * @param callable|bool $close
 */
function session_set_save_handler(
    $open,
    $close = UNKNOWN,
    callable $read = UNKNOWN,
    callable $write = UNKNOWN,
    callable $destroy = UNKNOWN,
    callable $gc = UNKNOWN,
    ?callable $create_sid = null,
    ?callable $validate_sid = null,
    ?callable $update_timestamp = null
): bool {}

/** @refcount 1 */
function session_cache_limiter(?string $value = null): string|false {}

function session_cache_expire(?int $value = null): int|false {}

function session_set_cookie_params(array|int $lifetime_or_options, ?string $path = null, ?string $domain = null, ?bool $secure = null, ?bool $httponly = null): bool {}

function session_start(array $options = []): bool {}

interface SessionHandlerInterface
{
    /** @tentative-return-type */
    public function open(string $path, string $name): bool;

    /** @tentative-return-type */
    public function close(): bool;

    /** @tentative-return-type */
    public function read(string $id): string|false;

    /** @tentative-return-type */
    public function write(string $id, string $data): bool;

    /** @tentative-return-type */
    public function destroy(string $id): bool;

    /** @tentative-return-type */
    public function gc(int $max_lifetime): int|false;
}

interface SessionIdInterface
{
    /** @tentative-return-type */
    public function create_sid(): string;
}

interface SessionUpdateTimestampHandlerInterface
{
    /** @tentative-return-type */
    public function validateId(string $id): bool;

    /** @tentative-return-type */
    public function updateTimestamp(string $id, string $data): bool;
}

class SessionHandler implements SessionHandlerInterface, SessionIdInterface
{
    /** @tentative-return-type */
    public function open(string $path, string $name): bool {}

    /** @tentative-return-type */
    public function close(): bool {}

    /** @tentative-return-type */
    public function read(string $id): string|false {}

    /** @tentative-return-type */
    public function write(string $id, string $data): bool {}

    /** @tentative-return-type */
    public function destroy(string $id): bool {}

    /** @tentative-return-type */
    public function gc(int $max_lifetime): int|false {}

    /** @tentative-return-type */
    public function create_sid(): string {}
}
