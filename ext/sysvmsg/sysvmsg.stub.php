<?php

/** @return resource|false */
function msg_get_queue(int $key, int $perms = 0666) {}

/**
 * @param resource $queue
 */
function msg_send($queue, int $msgtype, $message, bool $serialize = true, bool $blocking = true, &$errorcode = null): bool {}

/**
 * @param resource $queue
 */
function msg_receive($queue, int $desiredmsgtype, &$msgtype, int $maxsize, &$message, bool $unserialize = true, int $flags = 0, &$errorcode = null): bool {}

/**
 * @param resource $queue
 */
function msg_remove_queue($queue): bool {}

/**
 * @param resource $queue
 * @return array|false
 */
function msg_stat_queue($queue) {}

/**
 * @param resource $queue
 */
function msg_set_queue($queue, array $data): bool {}

function msg_queue_exists(int $key): bool {}
