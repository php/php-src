--TEST--
AF_PACKET socket_recvfrom() without optional port argument
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
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);

socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');

$dst_mac = "\xff\xff\xff\xff\xff\xff";
$src_mac = "\x00\x00\x00\x00\x00\x00";
$ethertype = pack("n", 0x9000);
$payload = "no port test";
$frame = str_pad($dst_mac . $src_mac . $ethertype . $payload, 60, "\x00");

// ETH_P_ALL sockets on loopback can observe unrelated localhost traffic from
// the parallel test runner. Read until we see our own frame, then validate the
// address returned by recvfrom() without the optional port argument.
function recv_matching(Socket $s, string $header, int $maxlen = 65536, ?string &$addr = null): string|false {
    socket_set_nonblock($s);
    $deadline = microtime(true) + 5.0;
    while (microtime(true) < $deadline) {
        $bytes = @socket_recvfrom($s, $buf, $maxlen, 0, $addr);
        if ($bytes !== false && is_string($buf) && str_starts_with($buf, $header)) {
            return $buf;
        }
        if ($bytes === false) {
            usleep(1000);
        }
    }
    return false;
}

socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);

// recvfrom without the optional 6th argument (port/ifindex).
$buf = recv_matching($s_recv, $dst_mac . $src_mac . $ethertype, 65536, $addr);
var_dump($buf !== false && strlen($buf) >= 60);
var_dump($addr === 'lo');

socket_close($s_send);
socket_close($s_recv);
?>
--EXPECT--
bool(true)
bool(true)
