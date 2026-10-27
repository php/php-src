--TEST--
Test TCP_CONGESTION constant with allowed algos for unprivileged accounts.
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('TCP_CONGESTION')) {
    die('skip TCP_CONGESTION test');
}
if (PHP_OS_FAMILY === 'Solaris' ) {
    die("skip Solaris does not allow setting TCP_CONGESTION");
}
?>
--FILE--
<?php

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) die ("socket failed");
$r = socket_get_option($socket, SOL_TCP, TCP_CONGESTION);
/* Use the algorithm currently in effect: it is guaranteed to be available and
 * allowed for unprivileged accounts, unlike "newreno" which is no longer loaded
 * by default on FreeBSD 14. */
$algo = $r['name'];
echo "current tcp congestion algo " . $algo . "\n";
var_dump(socket_set_option($socket, SOL_TCP, TCP_CONGESTION, $algo));
$r = socket_get_option($socket, SOL_TCP, TCP_CONGESTION);
echo "new tcp congestion algo " . $r['name'];
?>
--EXPECTF--
current tcp congestion algo %s
bool(true)
new tcp congestion algo %s
