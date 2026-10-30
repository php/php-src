--TEST--
Testing socket_get_status()
--FILE--
<?php

for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}

var_dump(socket_get_status($server));
fclose($server);

?>
--EXPECTF--
array(8) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(%d) "tcp_socket%S"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["uri"]=>
  string(21) "tcp://127.0.0.1:%d"
}
