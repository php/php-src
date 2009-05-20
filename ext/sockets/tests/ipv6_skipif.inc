<?php
if (!defined("AF_INET6")) {
	die('skip no IPv6 support');
}
/* If IPv6 is supported on the platform this will error out with code 111 - Connection refused.
   If IPv6 is NOT supported, $errno will be set to something else (indicating parse/getaddrinfo error) */
@stream_socket_client('tcp://[::1]:0', $errno);
if ($errno != 111) die('skip no IPv6 support');
