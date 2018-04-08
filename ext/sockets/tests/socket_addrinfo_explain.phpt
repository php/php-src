--TEST--
Test socket_addrinfo_explain()
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
    'ai_family' => AF_INET,
    'ai_socktype' => SOCK_DGRAM,
));
var_dump(socket_addrinfo_explain($addrinfo[0]));
echo "Done";
--EXPECTF--
array(5) {
  ["ai_flags"]=>
  int(%d)
  ["ai_family"]=>
  int(2)
  ["ai_socktype"]=>
  int(2)
  ["ai_protocol"]=>
  int(%d)
  ["ai_addr"]=>
  array(2) {
    ["sin_port"]=>
    int(2000)
    ["sin_addr"]=>
    string(9) "127.0.0.1"
  }
}
Done
