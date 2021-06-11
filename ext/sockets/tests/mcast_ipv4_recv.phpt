--TEST--
Multicast support: IPv4 receive options
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (getenv('SKIP_ONLINE_TESTS')) die('skip online test');
$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
$br = socket_bind($s, '0.0.0.0', 0);
$so = @socket_set_option($s, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group"	=> '224.0.0.23',
    "interface" => 'lo',
));
if ($so === false) {
    die('skip interface \'lo\' is unavailable.');
}
if (!defined("MCAST_BLOCK_SOURCE")) {
    die('skip source operations are unavailable');
}
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$domain = AF_INET;
$level = IPPROTO_IP;
$interface = "lo";
$mcastaddr = '224.0.0.23';
$sblock = "127.0.0.1";

echo "creating send socket bound to 127.0.0.1\n";
$sends1 = socket_create($domain, SOCK_DGRAM, SOL_UDP);
$br = socket_bind($sends1, '127.0.0.1');
var_dump($br);

echo "creating unbound socket and hoping the routing table causes an interface other than lo to be used for sending messages to $mcastaddr\n";
$sends2 = socket_create($domain, SOCK_DGRAM, SOL_UDP);
var_dump($br);

echo "creating receive socket\n";
$s = socket_create($domain, SOCK_DGRAM, SOL_UDP);
var_dump($s);
$br = socket_bind($s, '0.0.0.0', 0);
socket_getsockname($s, $unused, $port);
var_dump($br);

$so = socket_set_option($s, $level, MCAST_JOIN_GROUP, array(
    "group"	=> $mcastaddr,
    "interface" => $interface,
));
var_dump($so);

$r = socket_sendto($sends1, $m = "initial packet", strlen($m), 0, $mcastaddr, $port);
var_dump($r);

$i = 0;
checktimeout($s, 500);
while (($str = socket_read($s, 3000)) !== FALSE) {
    $i++;
    echo "$i> ", $str, "\n";

if ($i == 1) {
    echo "leaving group\n";
    $so = socket_set_option($s, $level, MCAST_LEAVE_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "ignored mcast packet", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
    $r = socket_sendto($sends1, $m = "unicast packet", strlen($m), 0, "127.0.0.1", $port);
    var_dump($r);
}
if ($i == 2) {
    echo "re-joining group\n";
    $so = socket_set_option($s, $level, MCAST_JOIN_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
    ));
    var_dump($so);
    $r = socket_sendto($sends2, $m = "ignored mcast packet (different interface)", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
    $r = socket_sendto($sends1, $m = "mcast packet", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
}
if ($i == 3) {
    echo "blocking source\n";
    $so = socket_set_option($s, $level, MCAST_BLOCK_SOURCE, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
        "source" => $sblock,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "ignored packet (blocked source)", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
    $r = socket_sendto($sends1, $m = "unicast packet", strlen($m), 0, "127.0.0.1", $port);
    var_dump($r);
}
if ($i == 4) {
    echo "unblocking source\n";
    $so = socket_set_option($s, $level, MCAST_UNBLOCK_SOURCE, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
        "source" => $sblock,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "mcast packet from 127.0.0.1", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
}
if ($i == 5) {
    echo "leaving group\n";
    $so = socket_set_option($s, $level, MCAST_LEAVE_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "ignored mcast packet", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
    $r = socket_sendto($sends1, $m = "unicast packet", strlen($m), 0, "127.0.0.1", $port);
    var_dump($r);
}
if ($i == 6) {
    echo "joining source group\n";
    $so = socket_set_option($s, $level, MCAST_JOIN_SOURCE_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
        "source" => $sblock,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "mcast packet from 127.0.0.1", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
}
if ($i == 7) {
    echo "leaving source group\n";
    $so = socket_set_option($s, $level, MCAST_LEAVE_SOURCE_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
        "source" => $sblock,
    ));
    var_dump($so);
    $r = socket_sendto($sends1, $m = "ignored mcast packet", strlen($m), 0, $mcastaddr, $port);
    var_dump($r);
    $r = socket_sendto($sends1, $m = "unicast packet", strlen($m), 0, "127.0.0.1", $port);
    var_dump($r);
}
if ($i == 8) {
/*	echo "rjsg\n";
    $so = socket_set_option($s, $level, MCAST_JOIN_GROUP, array(
        "group"	=> $mcastaddr,
        "interface" => $interface,
    ));
    var_dump($so);*/
    break;
}

}
?>
--EXPECTF--
creating send socket bound to 127.0.0.1
bool(true)
creating unbound socket and hoping the routing table causes an interface other than lo to be used for sending messages to 224.0.0.23
bool(true)
creating receive socket
object(Socket)#%d (0) {
}
bool(true)
bool(true)
int(14)
1> initial packet
leaving group
bool(true)
int(20)
int(14)
2> unicast packet
re-joining group
bool(true)
int(42)
int(12)
3> mcast packet
blocking source
bool(true)
int(31)
int(14)
4> unicast packet
unblocking source
bool(true)
int(27)
5> mcast packet from 127.0.0.1
leaving group
bool(true)
int(20)
int(14)
6> unicast packet
joining source group
bool(true)
int(27)
7> mcast packet from 127.0.0.1
leaving source group
bool(true)
int(20)
int(14)
8> unicast packet
