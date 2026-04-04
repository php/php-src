--TEST--
Test if socket_recvfrom() receives raw data sent by socket_sendto() via AF_PACKET
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php
if (!defined("AF_PACKET")) {
    die('SKIP AF_PACKET not supported on this platform.');
}
if (!defined("ETH_P_ALL")) {
    die('SKIP ETH_P_ALL not available on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
    die('SKIP AF_PACKET requires root permissions.');
}
?>
--FILE--
<?php

$dst_mac = "\xff\xff\xff\xff\xff\xff";
$src_mac = "\x00\x00\x00\x00\x00\x00";

// Helper to build a padded ethernet frame.
function build_frame(string $dst, string $src, int $ethertype, string $payload): string {
    $frame = $dst . $src . pack("n", $ethertype) . $payload;
    return str_pad($frame, 60, "\x00");
}

// Drain any pending packets from a socket.
function drain_socket(Socket $s): void {
    socket_set_nonblock($s);
    while (@socket_recvfrom($s, $buf, 65536, 0, $addr) !== false) {}
    socket_set_block($s);
}

echo "--- ETH_P_ALL send and receive ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$frame = build_frame($dst_mac, $src_mac, 0x9000, "ETH_P_ALL test");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent >= 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
var_dump(is_string($buf));
var_dump($addr === 'lo');
var_dump(str_contains($buf, "ETH_P_ALL test"));

socket_close($s_send);
socket_close($s_recv);

echo "--- ETH_P_LOOP send and receive ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$frame = build_frame($dst_mac, $src_mac, ETH_P_LOOP, "loopback payload");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent >= 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
// Verify ETH_P_LOOP ethertype at offset 12-13.
var_dump(unpack("n", $buf, 12)[1] === ETH_P_LOOP);
var_dump(str_contains($buf, "loopback payload"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Large payload ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$payload = random_bytes(1024);
$frame = build_frame($dst_mac, $src_mac, 0x9000, $payload);
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent >= strlen($frame));

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr, $port);
var_dump($bytes >= strlen($frame));
var_dump(is_int($port));
// Verify the payload is intact in the raw buffer.
var_dump(str_contains($buf, $payload));

socket_close($s_send);
socket_close($s_recv);
?>
--EXPECT--
--- ETH_P_ALL send and receive ---
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
--- ETH_P_LOOP send and receive ---
bool(true)
bool(true)
bool(true)
bool(true)
--- Large payload ---
bool(true)
bool(true)
bool(true)
bool(true)
