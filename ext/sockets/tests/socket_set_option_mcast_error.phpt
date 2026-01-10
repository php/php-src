--TEST--
Multicast error
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_OS == 'Darwin') die('skip Not for macOS');
?>
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$domain = AF_INET;
$level = IPPROTO_IP;
$interface = "lo";
$mcastaddr = '224.0.0.23';
$sblock = "127.0.0.1";

$s = socket_create($domain, SOCK_DGRAM, SOL_UDP);
$b = socket_bind($s, '0.0.0.0', 0);
$iwanttoleavenow = true;

try {
	socket_set_option($s, $level, MCAST_LEAVE_GROUP, $iwanttoleavenow);
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	socket_set_option($s, $level, MCAST_LEAVE_SOURCE_GROUP, $iwanttoleavenow);
} catch (\TypeError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
socket_set_option(): Argument #4 ($value) must be of type array when argument #3 ($option) is MCAST_LEAVE_GROUP, true given
socket_set_option(): Argument #4 ($value) must be of type array when argument #3 ($option) is MCAST_LEAVE_SOURCE_GROUP, true given
