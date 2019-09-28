--TEST--
Multicast support: IPv6 receive options (limited)
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip sockets extension not available.');
}
if (!defined('IPPROTO_IPV6')) {
	die('skip IPv6 not available.');
}
$s = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP);
$br = socket_bind($s, '::', 3000);
/* On Linux, there is no route ff00::/8 by default on lo, which makes it
 * troublesome to send multicast traffic from lo, which we must since
 * we're dealing with interface-local traffic... */
$so = @socket_set_option($s, IPPROTO_IPV6, MCAST_JOIN_GROUP, array(
	"group"	=> 'ff01::114',
	"interface" => 0,
));
if ($so === false) {
    die('skip unable to join multicast group on any interface.');
}
$r = socket_sendto($s, $m = "testing packet", strlen($m), 0, 'ff01::114', 3000);
if ($r === false) {
	die('skip unable to send multicast packet.');
}
$so = @socket_set_option($s, IPPROTO_IPV6, MCAST_LEAVE_GROUP, array(
	"group"	=> 'ff01::114',
	"interface" => 0,
));
if (defined("MCAST_JOIN_SOURCE_GROUP")) {
	$so = @socket_set_option($s, IPPROTO_IPV6, MCAST_JOIN_SOURCE_GROUP, array(
		"group"	=> 'ff01::114',
		"interface" => 0,
		"source" => '2001::dead:beef',
	));
	if ($so !== false) {
	    die('skip protocol independent multicast API is available.');
	}
}
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$domain = AF_INET6;
$level = IPPROTO_IPV6;
$interface = 0;
$mcastaddr = 'ff01::114';
$sblock = "?";

echo "creating send socket\n";
$sends1 = socket_create($domain, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump($sends1);

echo "creating receive socket\n";
$s = socket_create($domain, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump($s);
$br = socket_bind($s, '::0', 3000) or die("err");
var_dump($br);

$so = socket_set_option($s, $level, MCAST_JOIN_GROUP, array(
	"group"	=> $mcastaddr,
	"interface" => $interface,
)) or die("err");
var_dump($so);

$r = socket_sendto($sends1, $m = "testing packet", strlen($m), 0, $mcastaddr, 3000);
var_dump($r);
checktimeout($s, 500);
$r = socket_recvfrom($s, $str, 2000, 0, $from, $fromPort);
var_dump($r, $str, $from);
$sblock = $from;

$r = socket_sendto($sends1, $m = "initial packet", strlen($m), 0, $mcastaddr, 3000);
var_dump($r);

$i = 0;
checktimeout($s, 500);
while (($str = socket_read($s, 3000, 500)) !== FALSE) {
	$i++;
	echo "$i> ", $str, "\n";

if ($i == 1) {
	echo "leaving group\n";
	$so = socket_set_option($s, $level, MCAST_LEAVE_GROUP, array(
		"group"	=> $mcastaddr,
		"interface" => $interface,
	));
	var_dump($so);
	$r = socket_sendto($sends1, $m = "ignored mcast packet", strlen($m), 0, $mcastaddr, 3000);
	var_dump($r);
	$r = socket_sendto($sends1, $m = "unicast packet", strlen($m), 0, "::1", 3000);
	var_dump($r);
}
if ($i == 2) {
	echo "re-joining group\n";
	$so = socket_set_option($s, $level, MCAST_JOIN_GROUP, array(
		"group"	=> $mcastaddr,
		"interface" => $interface,
	));
	var_dump($so);
	$r = socket_sendto($sends1, $m = "mcast packet", strlen($m), 0, $mcastaddr, 3000);
	var_dump($r);
}
if ($i == 3) {
	break;
}

}
--EXPECTF--
creating send socket
resource(%d) of type (Socket)
creating receive socket
resource(%d) of type (Socket)
bool(true)
bool(true)
int(14)
int(14)
string(14) "testing packet"
string(%d) "%s"
int(14)
1> initial packet
leaving group
bool(true)
int(20)
int(14)
2> unicast packet
re-joining group
bool(true)
int(12)
3> mcast packet
