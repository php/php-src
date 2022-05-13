<?php

/** @generate-class-entries */

/**
 * @strict-properties
 * @not-serializable
 */
final class Fiber
{
    public function __construct(callable $callback) {}

    public function start(mixed ...$args): mixed {}

    public function resume(mixed $value = null): mixed {}

    public function throw(Throwable $exception): mixed {}

    public function isStarted(): bool {}

    public function isSuspended(): bool {}

    public function isRunning(): bool {}

    public function isTerminated(): bool {}

    public function getReturn(): mixed {}

    public static function getCurrent(): ?Fiber {}

    public static function suspend(mixed $value = null): mixed {}
}

final class FiberError extends Error
{
    public function __construct() {}
}
