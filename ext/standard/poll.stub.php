<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_POLL_READ
 */
const POLL_EVENT_READ = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_WRITE
 */
const POLL_EVENT_WRITE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ERROR
 */
const POLL_EVENT_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_HUP
 */
const POLL_EVENT_HUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_RDHUP
 */
const POLL_EVENT_RDHUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ONESHOT
 */
const POLL_EVENT_ONESHOT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ET
 */
const POLL_EVENT_ET = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_AUTO
 */
const POLL_BACKEND_AUTO = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_POLL
 */
const POLL_BACKEND_POLL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_EPOLL
 */
const POLL_BACKEND_EPOLL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_KQUEUE
 */
const POLL_BACKEND_KQUEUE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_EVENTPORT
 */
const POLL_BACKEND_EVENTPORT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_WSAPOLL
 */
const POLL_BACKEND_WSAPOLL = UNKNOWN;


abstract class PollHandle
{
    protected function getFileDescriptor(): int {}
}

final class StreamPollHandle extends PollHandle
{
    /** @param resource $stream */
    public function __construct($stream) {}

    /** @return resource */
    public function getStream() {}

    public function isValid(): bool {}
}

final class PollWatcher
{
    public function __construct(PollHandle $handle, int $events, mixed $data = null) {}

    public function getHandle(): PollHandle {}

    public function getWatchedEvents(): int {}

    public function getTriggeredEvents(): int {}

    public function getData(): mixed {}

    public function hasTriggered(int $events): bool {}

    public function isActive(): bool {}

    public function modify(int $events, mixed $data = null): void {}

    public function modifyEvents(int $events): void {}

    public function modifyData(mixed $data): void {}

    public function remove(): void {}
}

final class PollContext
{
    public function __construct(int|string $backend = POLL_BACKEND_AUTO) {}

    public function add(PollHandle $handle, int $events, mixed $data = null): PollWatcher {}

    public function wait(int $timeout = -1, int $maxEvents = -1): array {}

    public function getBackendName(): string {}
}

class PollException extends Exception
{
}
