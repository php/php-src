<?php
if (!defined("AF_INET6")) {
	die('skip no IPv6 support');
}
if (@stream_socket_client('udp://[::1]:8888') === false)
	die('skip no IPv6 support');
