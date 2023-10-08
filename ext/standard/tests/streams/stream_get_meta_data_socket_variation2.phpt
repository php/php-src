--TEST--
Testing stream_get_meta_data() "timed_out" field on a udp socket
--FILE--
<?php

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31332');

/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31332');
if (!$client) {
    die("Unable to create socket");
}

/* Accept that connection */
$socket = stream_socket_accept($server);

var_dump(stream_get_meta_data($client));

echo "\n\nSet a timeout on the client and attempt a read:\n";
socket_set_timeout($client, 0, 1000);
fread($client, 1);
var_dump(stream_get_meta_data($client));

echo "\n\nWrite some data from the server:\n";
fwrite($socket, "12345");
var_dump(stream_get_meta_data($client));

echo "\n\nRead some data from the client:\n";
fread($client, 5);
var_dump(stream_get_meta_data($client));

fclose($client);
fclose($socket);
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
  string(%d) "tcp_socke%s"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["uri"]=>
  string(21) "tcp://127.0.0.1:31332"
}


Set a timeout on the client and attempt a read:
array(8) {
  ["timed_out"]=>
  bool(true)
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
  ["uri"]=>
  string(21) "tcp://127.0.0.1:31332"
}


Write some data from the server:
array(8) {
  ["timed_out"]=>
  bool(true)
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
  ["uri"]=>
  string(21) "tcp://127.0.0.1:31332"
}


Read some data from the client:
array(8) {
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
  ["uri"]=>
  string(21) "tcp://127.0.0.1:31332"
}
