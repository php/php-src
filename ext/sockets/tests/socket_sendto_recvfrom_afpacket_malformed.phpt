--TEST--
AF_PACKET socket_sendto/socket_recvfrom with malformed and edge-case frames
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

// Drain any pending packets from a socket.
function drain_socket(Socket $s): void {
    socket_set_nonblock($s);
    while (@socket_recvfrom($s, $buf, 65536, 0, $addr) !== false) {}
    socket_set_block($s);
}

echo "--- Undersized frame (below 14-byte ethernet header) ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
// Kernel requires at least an ethernet header, sendto should fail.
$tiny = "\xff\xff\xff\xff";
$ret = @socket_sendto($s_send, $tiny, strlen($tiny), 0, "lo", 1);
var_dump($ret === false);
socket_close($s_send);

echo "--- Zero-length payload (header only, padded to 60) ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

// Ethernet header with no payload, padded to minimum 60 bytes.
$frame = str_pad($dst_mac . $src_mac . pack("n", 0x9000), 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
// The raw buffer is just padding after the header.
var_dump(strlen($buf) >= 60);

socket_close($s_send);
socket_close($s_recv);

echo "--- Bogus ethertype ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

// Use a made-up ethertype (0xBEEF). Kernel delivers it fine on loopback.
$frame = str_pad($dst_mac . $src_mac . pack("n", 0xBEEF) . "bogus", 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
// Ethertype bytes should be in the raw buffer at offset 12-13.
var_dump(unpack("n", $buf, 12)[1] === 0xBEEF);
var_dump(str_contains($buf, "bogus"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Garbage payload with custom ethertype ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

// Use a non-standard ethertype (0x88B5, reserved for local experimental use)
// with garbage payload. Avoids kernel IP/IPv6 stack interception.
$frame = str_pad($dst_mac . $src_mac . pack("n", 0x88B5) . "\xDE\xAD\xBE\xEF", 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
// Raw buffer is delivered as-is — PHP doesn't parse, so no crash.
var_dump(str_contains($buf, "\xDE\xAD\xBE\xEF"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Another garbage payload with experimental ethertype ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

// Use 0x88B6, another local experimental ethertype.
$frame = str_pad($dst_mac . $src_mac . pack("n", 0x88B6) . "\xCA\xFE", 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
// Delivered raw — no parsing, no crash.
var_dump(str_contains($buf, "\xCA\xFE"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Invalid ethertype 0x0000 ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$frame = str_pad($dst_mac . $src_mac . pack("n", 0x0000) . "zerotype", 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
var_dump(unpack("n", $buf, 12)[1] === 0x0000);
var_dump(str_contains($buf, "zerotype"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Invalid ethertype 0xFFFF ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$frame = str_pad($dst_mac . $src_mac . pack("n", 0xFFFF) . "maxtype", 60, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 60);

$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
var_dump(unpack("n", $buf, 12)[1] === 0xFFFF);
var_dump(str_contains($buf, "maxtype"));

socket_close($s_send);
socket_close($s_recv);

echo "--- Small receive buffer (truncation) ---\n";
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');
drain_socket($s_recv);

$payload = str_repeat("X", 200);
$frame = str_pad($dst_mac . $src_mac . pack("n", 0x9000) . $payload, 214, "\x00");
$sent = socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);
var_dump($sent === 214);

// Request only 30 bytes — less than the frame. Kernel truncates.
$bytes = socket_recvfrom($s_recv, $buf, 30, 0, $addr);
var_dump($bytes === 30);
var_dump(strlen($buf) === 30);

socket_close($s_send);
socket_close($s_recv);
?>
--EXPECT--
--- Undersized frame (below 14-byte ethernet header) ---
bool(true)
--- Zero-length payload (header only, padded to 60) ---
bool(true)
bool(true)
bool(true)
--- Bogus ethertype ---
bool(true)
bool(true)
bool(true)
bool(true)
--- Garbage payload with custom ethertype ---
bool(true)
bool(true)
bool(true)
--- Another garbage payload with experimental ethertype ---
bool(true)
bool(true)
bool(true)
--- Invalid ethertype 0x0000 ---
bool(true)
bool(true)
bool(true)
bool(true)
--- Invalid ethertype 0xFFFF ---
bool(true)
bool(true)
bool(true)
bool(true)
--- Small receive buffer (truncation) ---
bool(true)
bool(true)
bool(true)
