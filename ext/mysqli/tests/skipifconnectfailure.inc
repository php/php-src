<?php
require_once 'connect.inc';
require_once dirname(__DIR__, 3) . '/tests/probe_cache.inc';

$configuration = [$host, $port, $user, $passwd, $db, $socket, get_environment_connection_flags()];

$reason = ProbeCache::getFailure('mysqli', $configuration, static function () use ($host, $user, $passwd, $db, $port, $socket): ?string {
    $link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
    if (!is_object($link)) {
        return sprintf("Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error());
    }

    mysqli_close($link);
    return null;
});

if (is_string($reason)) {
    die("skip $reason");
}
?>
