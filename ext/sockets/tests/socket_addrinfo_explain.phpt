--TEST--
Test socket_addrinfo_explain()
--EXTENSIONS--
sockets
--FILE--
<?php
$addrinfo = socket_addrinfo_lookup('127.0.0.1', 2000, array(
    'ai_family' => AF_INET,
    'ai_socktype' => SOCK_DGRAM,
));
$result = socket_addrinfo_explain($addrinfo[0]);
// Musl sets ai_canonname even if AI_CANONNAME is not specified.
unset($result['ai_canonname']);
var_dump($result);
// Solaris uses different numeric values for SOCK_* constants,
// so avoid comparing raw integers in EXPECTF.
if ($result['ai_socktype'] != SOCK_DGRAM) {
    echo "Wrong socktype\n";
}
echo "Done";
?>
--EXPECTF--
array(5) {
  ["ai_flags"]=>
  int(%d)
  ["ai_family"]=>
  int(2)
  ["ai_socktype"]=>
  int(%s)
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
