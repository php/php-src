--TEST--
Testing stream_get_meta_data() "unread_bytes" field on a udp socket
--FILE--
<?php

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31331');

/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31331');
if (!$client) {
	die("Unable to create socket");
}

/* Accept that connection */
$socket = stream_socket_accept($server);

echo "Write some data:\n";
fwrite($socket, "abcdefg\n1234567\nxyzxyz\n");
var_dump(stream_get_meta_data($client));

echo "\n\nRead a line from the client, causing data to be buffered:\n";
fgets($client);
var_dump(stream_get_meta_data($client));

echo "\n\nRead 3 bytes of data from the client:\n";
fread($client, 3);
var_dump(stream_get_meta_data($client));

echo "\n\nClose the server side socket and read the remaining data from the client:\n";
fclose($socket);
fclose($server);
while(!feof($client)) {
	fread($client, 1);
}
var_dump(stream_get_meta_data($client));

?>
--EXPECTF--
Write some data:
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


Read a line from the client, causing data to be buffered:
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
  int(15)
  ["seekable"]=>
  bool(false)
}


Read 3 bytes of data from the client:
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
  int(12)
  ["seekable"]=>
  bool(false)
}


Close the server side socket and read the remaining data from the client:
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(true)
  ["stream_type"]=>
  string(%d) "tcp_socke%s"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
}
