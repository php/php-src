--TEST--
Test if socket_recvfrom() receives data sent by socket_sendto() via IPv4 UDP
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if (!$socket) {
    die('Unable to create AF_INET socket');
}

if (!socket_bind($socket, '127.0.0.1', 0)) {
    die(sprintf(
        'An error occurred while binding socket: %s',
        socket_strerror(socket_last_error($socket))));
}

socket_getsockname($socket, $address, $port);

$msg = "Ping!";
$len = strlen($msg);
$sent = socket_sendto($socket, $msg, $len, 0, $address, $port);
if ($sent === false) {
    die(sprintf(
        'An error occurred while sending to the socket: %s',
        socket_strerror(socket_last_error($socket))));
} else if ($sent != $len) {
    die(sprintf(
        '%d bytes have been sent instead of the %d bytes expected',
        $sent, $len));
}

$wants = $len;
$recvd = 0;
$buf   = null;

while ($recvd < $len) {
    $bytes = socket_recvfrom(
        $socket, $buffering, $wants, 0, $address, $port);

    if (($bytes === false) && ($errno = socket_last_error($socket))) {
        if ($errno = SOCKET_EAGAIN) {
            socket_clear_error($socket);
            continue;
        }

        die(sprintf(
            'An error occurred while sending to the socket: %s',
            socket_strerror($errno)));
    }

    $recvd += $bytes;
    $wants -= $bytes;
    $buf .= $buffering;
}

if ($recvd != $len) {
    die(sprintf(
        '%d bytes have been received instead of the %d bytes expected',
        $recvd, $len));
}

echo "Received $buf from remote address $address and remote port $port" . PHP_EOL;
?>
--EXPECTF--
Received Ping! from remote address %s and remote port %d
