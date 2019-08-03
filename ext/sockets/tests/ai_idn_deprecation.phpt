--TEST--
AI_IDN_ALLOW_UNASSIGNED and AI_IDN_USE_STD3_ASCII_RULES are deprecated
--SKIPIF--
<?php
if (!extension_loaded('sockets')) die('skip The sockets extension is not loaded');
if (!defined('AI_IDN_ALLOW_UNASSIGNED')) die('skip AI_IDN_ALLOW_UNASSIGNED not defined');
?>
--FILE--
<?php
$addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
    'ai_family' => AF_INET,
    'ai_socktype' => SOCK_DGRAM,
    'ai_flags' => AI_IDN_ALLOW_UNASSIGNED,
));
var_dump(socket_addrinfo_connect($addrinfo[0]));
echo "Done";
--EXPECTF--
Deprecated: socket_addrinfo_lookup(): AI_IDN_ALLOW_UNASSIGNED and AI_IDN_USE_STD3_ASCII_RULES are deprecated in %s on line %d
resource(%d) of type (Socket)
Done
