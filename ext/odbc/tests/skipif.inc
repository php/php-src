<?php

include 'config.inc';
require_once dirname(__DIR__, 3) . '/tests/probe_cache.inc';

try {
    $conn = ProbeCache::getFailure('odbc', [$dsn, $user, $pass], static function () use ($dsn, $user, $pass): Odbc\Connection {
        $conn = @odbc_connect($dsn, $user, $pass);
        if (!$conn) {
            throw new ProbeFailureException('could not connect');
        }

        return $conn;
    });
} catch (ProbeFailureException $e) {
    die("skip {$e->getMessage()}");
}
