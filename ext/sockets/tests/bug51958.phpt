--TEST--
Bug #51958: socket_accept() fails on IPv6 server sockets
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip sockets extension not available.');
}
if (!defined('IPPROTO_IPV6')) {
	die('skip IPv6 not available.');
}
if (PHP_OS != "WINNT")
	die('skip test relies Winsock\'s error code for WSAEWOULDBLOCK/EAGAIN');
--FILE--
<?php
$listenfd = socket_create(AF_INET6, SOCK_STREAM, SOL_TCP);
socket_bind($listenfd, "::1", 13579);
socket_listen($listenfd);
socket_set_nonblock($listenfd);
$connfd = @socket_accept($listenfd);
echo socket_last_error();
--EXPECT--
10035
