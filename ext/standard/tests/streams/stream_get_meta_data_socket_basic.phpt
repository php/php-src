--TEST--
stream_get_meta_data() on a udp socket
--FILE--
<?php

$tcp_socket = stream_socket_server('tcp://127.0.0.1:31330');
var_dump(stream_get_meta_data($tcp_socket));
fclose($tcp_socket);

?>
--EXPECTF--
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(%d) "tcp_socke%s"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
}
