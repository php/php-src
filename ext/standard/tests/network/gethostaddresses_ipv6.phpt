--TEST--
gethostaddresses() with an IPv6 literal
--SKIPIF--
<?php
if (@stream_socket_server("tcp://[::1]:0") === false) {
    die("skip IPv6 not available");
}
?>
--FILE--
<?php
var_dump(gethostaddresses("::1"));
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "::1"
}
