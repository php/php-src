<?php

/** @generate-function-entries */

function session_name(?string $name = null): string|false {}

function session_module_name(?string $module = null): string|false {}

function session_save_path(?string $path = null): string|false {}

function session_id(?string $id = null): string|false {}

function session_create_id(string $prefix = ""): string|false {}

function session_regenerate_id(bool $delete_old_session = false): bool {}

function session_decode(string $data): bool {}

function session_encode(): string|false {}

function session_destroy(): bool {}

function session_unset(): bool {}

function session_gc(): int|false {}

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
 * @param callable $read
 * @param callable $write
 * @param callable $destroy
 * @param callable $gc
 * @param callable $create_sid
 * @param callable $validate_sid
 * @param callable $update_timestamp
 */
function session_set_save_handler($open, $close = UNKNOWN, $read = UNKNOWN, $write = UNKNOWN, $destroy = UNKNOWN, $gc = UNKNOWN, $create_sid = UNKNOWN, $validate_sid = UNKNOWN, $update_timestamp = UNKNOWN): bool {}

function session_cache_limiter(?string $cache_limiter = null): string|false {}

function session_cache_expire(?int $new_cache_expire = null): int|false {}

/** @param int|array $lifetime_or_options */
function session_set_cookie_params($lifetime_or_options, string $path = UNKNOWN, string $domain  = "", ?bool $secure = null, ?bool $httponly = null): bool {}

function session_start(array $options = []): bool {}

interface SessionHandlerInterface
{
    /** @return bool */
    public function open(string $save_path, string $session_name);

    /** @return bool */
    public function close();

    /** @return string */
    public function read(string $key);

    /** @return bool */
    public function write(string $key, string $val);

    /** @return bool */
    public function destroy(string $key);

    /** @return int|bool */
    public function gc(int $maxlifetime);
}

interface SessionIdInterface
{
    /** @return string */
    public function create_sid();
}

interface SessionUpdateTimestampHandlerInterface
{
    /** @return bool */
    public function validateId(string $key);

    /** @return bool */
    public function updateTimestamp(string $key, string $val);
}

class SessionHandler implements SessionHandlerInterface, SessionIdInterface
{
    /** @return bool */
    public function open(string $save_path, string $session_name) {}

    /** @return bool */
    public function close() {}

    /** @return string */
    public function read(string $key) {}

    /** @return bool */
    public function write(string $key, string $val) {}

    /** @return bool */
    public function destroy(string $key) {}

    /** @return int|bool */
    public function gc(int $maxlifetime) {}

    /** @return string */
    public function create_sid() {}
}
