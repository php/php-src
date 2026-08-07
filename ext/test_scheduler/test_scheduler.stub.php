<?php

/** @generate-class-entries */

namespace TestScheduler;

/**
 * The scheduler's coroutine: the opaque object the Async Core hooks speak
 * in. Minted by spawn() only.
 *
 * @strict-properties
 * @not-serializable
 */
final class Coroutine
{
    private function __construct() {}

    public function isStarted(): bool {}

    public function isRunning(): bool {}

    public function isSuspended(): bool {}

    public function isFinished(): bool {}

    /** The body's return value; null until the coroutine finishes. */
    public function getResult(): mixed {}

    /** The exception the body finished with, or null. */
    public function getException(): ?\Throwable {}

    /**
     * What the suspended coroutine is waiting for: a list of human-readable
     * descriptions, one per registered wait. Empty when the coroutine is not
     * waiting for anything it can name.
     *
     * @return list<string>
     */
    public function getAwaitingInfo(): array {}
}

/**
 * Thrown into a cancelled coroutine at its suspend point. Catchable for
 * cleanup, but a cancelled coroutine cannot suspend again.
 */
class CancellationError extends \Error
{
}

/** Terminal: raised as the exit exception, catching it does not help. */
final class DeadlockError extends \Error
{
}

/** Queue a coroutine. The scheduler starts on the first call. */
function spawn(callable $callback, mixed ...$args): Coroutine {}

/** Yield the current flow (the main one included) to the scheduler. */
function suspend(): void {}

/** Put a suspended coroutine back into the run queue. */
function resume(Coroutine $coroutine): void {}

/** Throw a CancellationError at the coroutine's suspend point; repeats are no-ops. */
function cancel(Coroutine $coroutine): void {}

/**
 * Park the current coroutine until $coroutine finishes; returns its result.
 * An exception the coroutine finished with is rethrown here instead.
 */
function await(Coroutine $coroutine): mixed {}

/** The coroutine running right now, or null outside one. */
function current(): ?Coroutine {}
