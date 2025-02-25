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
if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
?>
--FILE--
<?php
    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');
    var_dump($s_bind);

    // sock_getsockname in this context gets the interface rather than the address.
    $s_conn  = socket_getsockname($s_c, $istr, $iindex);
    var_dump($s_conn);
    var_dump($istr);
    var_dump($iindex);

    socket_getpeername($s_c, $istr2, $iindex2);

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_LOOP);
    $v_bind  = socket_bind($s_s, 'lo');

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_LOOP);
    $buf .= str_repeat("A", 46);

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));
    var_dump(socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr));

    var_dump($addr);
    var_dump($rsp);

    socket_close($s_c);
    // purposely unsupported ethernet protocol (ARP)

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, 0x0806);
    $s_bind  = socket_bind($s_c, 'lo');
    $buf = pack("H12H12n", "ffffffffffff", "000000000000", 0x0806);
    $buf .= str_repeat("A", 46);

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
    	socket_recvfrom($s_c, $rsp2, strlen($buf), 0, $addr2);
    } catch (\ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
 
    socket_close($s_c);
    socket_close($s_s);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IP);
    $v_bind  = socket_bind($s_s, 'lo');

    $ip = hex2bin(
	    "4500" .
	    "0028" .
	    "0000" .
	    "4000" .
	    "4006" .
	    "0000" .
	    "7f000001" .
	    "7f000001"
    );
    $p = str_repeat("A", 20);

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_IP);
    $buf .= $ip . $p;

    $min_frame_size = 60;
    $buf .= str_repeat("\x00", max(0, $min_frame_size - strlen($buf)));

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));
    try {
	    socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
	    echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IPV6);
    $s_bind  = socket_bind($s_c, 'lo');
    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IPV6);
    $v_bind  = socket_bind($s_s, 'lo');

    $ethhdr = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_IPV6);

    $ipv6_first_4 = hex2bin("60000000");

    $ipv6_payload_len = hex2bin("0014");

    $ipv6_nexthdr = chr(6);

    $ipv6_hop_limit = chr(64);

    $src_ip = hex2bin("00000000000000000000000000000001");

    $dst_ip = hex2bin("00000000000000000000000000000001");

    $tcp_hdr = pack("nnNNCCnn",
	    12345,
	    80,
	    0,
	    0,
	    (5 << 4),
	    0x02,
	    65535,
	    0 
    );

    $buf = $ethhdr
	    . $ipv6_first_4
	    . $ipv6_payload_len
	    . $ipv6_nexthdr
	    . $ipv6_hop_limit
	    . $src_ip
	    . $dst_ip
	    . $tcp_hdr;

    $buf .= str_repeat("\x00", max(0, 60 - strlen($buf)));

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));
    var_dump(socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr));

    var_dump($addr);
    var_dump($rsp);

    $first_4_bytes = hex2bin("60000000");

    $payload_len = hex2bin("0014");

    $nexthdr = hex2bin("43");  // Extended proto

    $hop_limit = hex2bin("40");

    $src_ip = hex2bin("00000000000000000000000000000001");

    $dst_ip = hex2bin("00000000000000000000000000000001");

    $tcp_payload = str_repeat("A", 20);

    $ethhdr = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_IPV6);

    $buf = $ethhdr . $first_4_bytes . $payload_len . $nexthdr . $hop_limit . $src_ip . $dst_ip . $tcp_payload;

    $buf .= str_repeat("\x00", max(0, 60 - strlen($buf)));

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));
    
    try {
    	socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
	    echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IP);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IP);
    $v_bind  = socket_bind($s_s, 'lo');

    $ip = hex2bin(
	    "4500" .
	    str_repeat("0028", 16) .
	    "0000" .
	    "4000" .
	    "4006" .
	    "0000" .
	    "7f000001" .
	    "7f000001"
    );
    $p = str_repeat("A", 20);

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_IP);
    $buf .= $ip . $p;

    $min_frame_size = 60;
    $buf .= str_repeat("\x00", max(0, $min_frame_size - strlen($buf)));

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
    	socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
	    echo $e->getMessage(), PHP_EOL;
    }

    $ip = hex2bin(
	    "9999" .
	    "0028" .
	    "0000" .
	    "4000" .
	    "4006" .
	    "0000" .
	    "FFFFFeFF" .
	    "7f000001"
    );
    $p = str_repeat("Bb", 80);

    $buf = pack("H12H12n", "ffffffffffffh", "aaaaaAAAAAAA", ETH_P_IP);
    $buf .= $ip . $p;

    $min_frame_size = 60;
    $buf .= str_repeat("\x00", max(0, $min_frame_size - strlen($buf)));

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
	    socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
	    echo $e->getMessage(), PHP_EOL;
    }
 
    socket_close($s_s);
    socket_close($s_c);
?>
--EXPECTF--
bool(true)
bool(true)
string(2) "lo"
int(%i)

Warning: socket_getpeername(): unable to retrieve peer name [95]: %sot supported in %s on line %d
int(60)
int(60)
string(2) "lo"
object(Socket\EthernetPacket)#3 (%d) {
  ["headerSize"]=>
  int(%d)
  ["rawPacket"]=>
  string(%d) "%r(.|\n)*?"%r
  ["socket"]=>
  object(Socket)#1 (0) {
  }
  ["ethProtocol"]=>
  int(%i)
  ["srcMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["dstMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["payload"]=>
  %a
}
int(60)
unsupported ethernet protocol
int(60)
int(%d)
string(2) "lo"
object(Socket\EthernetPacket)#2 (7) {
  ["headerSize"]=>
  int(14)
  ["rawPacket"]=>
  string(%d) "%r(.|\n)*?"%r
  ["socket"]=>
  object(Socket)#%d (0) {
  }
  ["ethProtocol"]=>
  int(%d)
  ["srcMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["dstMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["payload"]=>
  object(Socket\Ipv%sPacket)#%d (5) {
    ["headerSize"]=>
    int(%d)
    ["rawPacket"]=>
    string(%d) "%r(.|\n)*?"%r
    ["srcAddr"]=>
    string(%d) "%s"
    ["dstAddr"]=>
    string(%d) "%s"
    ["payload"]=>
    object(Socket\%sPacket)#%d (4) {
      ["headerSize"]=>
      int(%d)
      ["rawPacket"]=>
      string(%d) "%r(.|\n)*?"%r
      ["srcPort"]=>
      int(%d)
      ["dstPort"]=>
      int(%d)
    }
  }
}
int(%d)
int(%d)
string(2) "lo"
object(Socket\EthernetPacket)#%d (7) {
  ["headerSize"]=>
  int(14)
  ["rawPacket"]=>
  string(%d) "%r(.|\n)*?"%r
  ["socket"]=>
  object(Socket)#3 (0) {
  }
  ["ethProtocol"]=>
  int(%d)
  ["srcMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["dstMac"]=>
  string(%d) "%s:%s:%s:%s:%s:%s"
  ["payload"]=>
  object(Socket\Ipv6Packet)#%d (5) {
    ["headerSize"]=>
    int(%d)
    ["rawPacket"]=>
    string(%d) "%A"
    ["srcAddr"]=>
    string(%d) "%s"
    ["dstAddr"]=>
    string(%d) "%s"
    ["payload"]=>
    object(Socket\TcpPacket)#%d (4) {
      ["headerSize"]=>
      int(%d)
      ["rawPacket"]=>
      string(%d) "%r(.|\n)*?"%r
      ["srcPort"]=>
      int(%d)
      ["dstPort"]=>
      int(%d)
    }
  }
}
int(%d)
unsupported ipv6 header protocol
int(%d)
%A
int(%d)
%A
string(%d) "%s"
object(Socket\EthernetPacket)#%d (7) {
  ["headerSize"]=>
  int(%d)
  ["rawPacket"]=>
  string(%d) "%r(.|\n)*?"%r
  ["socket"]=>
  object(Socket)#%d (0) {
  }
  ["ethProtocol"]=>
  int(2048)
  ["srcMac"]=>
  string(%d) "%s"
  ["dstMac"]=>
  string(%d) "%s"
  ["payload"]=>
  object(Socket\Ipv4Packet)#%d (5) {
    ["headerSize"]=>
    int(%d)
    ["rawPacket"]=>
    string(%d) "%s"
    ["srcAddr"]=>
    string(%d) "%s.%s.%s.%s"
    ["dstAddr"]=>
    string(%d) "%s.%s.%s.%s"
    ["payload"]=>
    object(Socket\TcpPacket)#%d (4) {
      ["headerSize"]=>
      int(%d)
      ["rawPacket"]=>
      string(%d) "%r(.|\n)*?"%r
      ["srcPort"]=>
      int(%d)
      ["dstPort"]=>
      int(%d)
    }
  }
}
