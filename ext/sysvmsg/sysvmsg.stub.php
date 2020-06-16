<?php

/** @generate-function-entries */

final class SysvMessageQueue
{
}

function msg_get_queue(int $key, int $perms = 0666): SysvMessageQueue|false {}

function msg_send(SysvMessageQueue $queue, int $msgtype, $message, bool $serialize = true, bool $blocking = true, &$errorcode = null): bool {}

function msg_receive(SysvMessageQueue $queue, int $desiredmsgtype, &$msgtype, int $maxsize, &$message, bool $unserialize = true, int $flags = 0, &$errorcode = null): bool {}

function msg_remove_queue(SysvMessageQueue $queue): bool {}

function msg_stat_queue(SysvMessageQueue $queue): array|false {}

function msg_set_queue(SysvMessageQueue $queue, array $data): bool {}

function msg_queue_exists(int $key): bool {}
