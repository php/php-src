--TEST--
socket_getsockname from AF_PACKET socket
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php

if (!defined("AF_PACKET")) {
    die('SKIP AF_PACKET not supported on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
    die('SKIP AF_PACKET requires root permissions.');
}
if (PHP_INT_SIZE != 4) die("skip this test is for 32-bit only");
?>
--FILE--
<?php
    $receiver = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $sender = socket_create(AF_PACKET, SOCK_RAW, ETH_P_LOOP);

    var_dump(socket_bind($receiver, 'lo'));

    var_dump(socket_getsockname($receiver, $if_name, $if_index));
    var_dump($if_name);
    var_dump(is_int($if_index));

    var_dump(socket_bind($sender, 'lo'));

    $payload = str_repeat("A", 46);
    $eth_frame = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_LOOP) . $payload;

    var_dump(socket_sendto($sender, $eth_frame, strlen($eth_frame), 0, "lo", 1));

    usleep(10000);

    $len = socket_recvfrom($receiver, $rsp, 1500, 0, $addr);
    var_dump($len);
    var_dump($addr);
    socket_close($sender);

    $arp_receiver = socket_create(AF_PACKET, SOCK_RAW, 0x0806); // ETH_P_ARP
    var_dump(socket_bind($arp_receiver, 'lo'));

    socket_set_option($arp_receiver, SOL_SOCKET, SO_RCVTIMEO, ["sec" => 0, "usec" => 50000]);

    $recv_result = socket_recvfrom($arp_receiver, $rsp2, 1500, 0, $addr2);
    var_dump($recv_result);
    if ($recv_result === false) {
        echo "Socket receive timed out as expected." . PHP_EOL;
    }
    socket_close($arp_receiver);

    $sender = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IP);
    var_dump(socket_bind($sender, 'lo'));

    $malformed_eth_header = pack("H12H12n", "001122334455", "AABBCCDDEEFF", ETH_P_IP);
    $malformed_ip_header = hex2bin(
        "9999" .
        "0028" .
        "0000" .
        "4000" .
        "4006" .
        "0000" .
        "7f000001" .
        "7f000001"
    );
    $malformed_payload = str_repeat("B", 20);
    $malformed_frame = $malformed_eth_header . $malformed_ip_header . $malformed_payload;
    $malformed_frame .= str_repeat("\x00", 60 - strlen($malformed_frame));

    var_dump(socket_sendto($sender, $malformed_frame, strlen($malformed_frame), 0, "lo", 1));

    usleep(10000);

    $len = socket_recvfrom($receiver, $rsp, 1500, 0, $addr);
    var_dump($len);
    var_dump($addr);
    var_dump($rsp);

    var_dump($rsp->ethProtocol === 0x0060);
    var_dump($rsp->payload->rawPacket !== NULL);

    var_dump(bin2hex($rsp->rawPacket));
    var_dump(bin2hex($rsp->payload->rawPacket));

    socket_close($sender);
    socket_close($receiver);
?>
--EXPECTF--
bool(true)
bool(true)
string(2) "lo"
bool(true)
bool(true)
int(60)
int(60)
string(2) "lo"
bool(true)
bool(false)
Socket receive timed out as expected.
bool(true)
int(60)
int(60)
string(2) "lo"
object(Socket\EthernetPacket)#2 (7) {
  ["headerSize"]=>
  int(14)
  ["rawPacket"]=>
  string(%d) "%r(.|\n)*?"%r
  ["socket"]=>
  object(Socket)#1 (0) {
  }
  ["ethProtocol"]=>
  int(%d)
  ["srcMac"]=>
  string(%d) "%s"
  ["dstMac"]=>
  string(%d) "%s"
  ["payload"]=>
  object(Socket\%sPacket)#%d (%d) {
    ["headerSize"]=>
    int(%d)
    ["rawPacket"]=>
    string(%d) "%s"
    ["socket"]=>
    object(Socket)#1 (0) {
    }
    ["ethProtocol"]=>
    int(0)
    ["src%sc"]=>
    string(%d) "%s"
    ["dst%s"]=>
    string(%d) "%s"
    ["payload"]=>
    %a
  }
}
bool(true)
bool(true)
string(120) "ffffffffffff000000000000006041414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141"
string(28) "4141414141414141414141414141"
