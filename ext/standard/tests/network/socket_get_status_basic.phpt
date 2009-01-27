--TEST--
Testing socket_get_status()
--FILE--
<?php

$tcp_socket = stream_socket_server('tcp://127.0.0.1:31337');
var_dump(socket_get_status($tcp_socket));
fclose($tcp_socket);

?>
--EXPECTF--
array(7) {
  ["stream_type"]=>
  string(%d) "tcp_socket%S"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
