<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_POLL_READ
 */
const STREAM_POLL_READ = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_WRITE
 */
const STREAM_POLL_WRITE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ERROR
 */
const STREAM_POLL_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_HUP
 */
const STREAM_POLL_HUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_RDHUP
 */
const STREAM_POLL_RDHUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ONESHOT
 */
const STREAM_POLL_ONESHOT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_ET
 */
const STREAM_POLL_ET = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_AUTO
 */
const STREAM_POLL_BACKEND_AUTO = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_POLL
 */
const STREAM_POLL_BACKEND_POLL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_EPOLL
 */
const STREAM_POLL_BACKEND_EPOLL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_KQUEUE
 */
const STREAM_POLL_BACKEND_KQUEUE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_EVENTPORT
 */
const STREAM_POLL_BACKEND_EVENTPORT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_SELECT
 */
const STREAM_POLL_BACKEND_SELECT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_POLL_BACKEND_IOCP
 */
const STREAM_POLL_BACKEND_IOCP = UNKNOWN;

final class StreamPollContext
{
}

final class StreamPollEvent
{
    /**
     * @var resource
     * @readonly
     */
    public $stream;
    
    /** @readonly */
    public int $events;
    
    /** @readonly */
    public mixed $data;
}

class StreamPollException extends Exception
{
}

function stream_poll_create(?int $max_events = null, int $backend = STREAM_POLL_BACKEND_AUTO): StreamPollContext {}

/** @param resource $stream */
function stream_poll_add(StreamPollContext $poll_ctx, $stream, int $events, mixed $data = null): void {}

/** @param resource $stream */
function stream_poll_modify(StreamPollContext $poll_ctx, $stream, int $events, mixed $data = null): void {}

/** @param resource $stream */
function stream_poll_remove(StreamPollContext $poll_ctx, $stream): void {}

function stream_poll_wait(StreamPollContext $poll_ctx, int $timeout = -1): array {}

function stream_poll_backend_name(StreamPollContext $poll_ctx): string {}
